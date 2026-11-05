# Type-safe guest address translation implementation plan

Status: staged implementation in progress, 2026-11-05. Strong pointers, typed
production callback dispatch, logical allocation and native recovery/snapshot
handles are implemented. Ordinary data-pointer APIs are not yet migrated; the
production runtime is not safe at high native addresses. See the [implementation/removal ledger](GUEST-ADDRESSING-MIGRATION.md). This follows the actual Apple Silicon probe results:
16 KiB pages; native code/globals/stack above 4 GiB; no low arena from ordinary
hints; MAP_32BIT fails with ENOMEM. Guest layout, native C++, nested jumps and
basic ucontext switching pass. The next port should translate addresses rather
than depend on low native placement. See [the cross-build work](MACOS-CROSS.md)
and [addressing background](ADDRESSING.md).

The non-negotiable contract is that passing a host pointer where a guest pointer
is required, or the reverse, is a C++ compilation error. This applies on Linux,
Windows, both Mac architectures and Amiga, even if a particular backend happens
to use identical numerical addresses. It must remain true in optimized builds.

## 1. Scope and current hazards

A text search of current `src/*.{h,cpp}` finds 1,801 `ptr<T>` matches in 151 files,
1,225 `address()` matches in 138 files, and 3,264 `reinterpret_cast` matches in
201 files. These include comments and are planning indicators, not an AST-derived
migration count. The 94 `address(&...)` matches include guest subobjects as well
as stack escapes; classify them individually.

The principal boundaries needing redesign are:

| Location | Current representation | Required replacement |
|---|---|---|
| `core/amos.h` | `ptr(uint32_t) -> T*`, `address(T*) -> uint32_t` | Strong guest addresses/views, no general native-pointer encoder |
| `core/layout.h`, `core/ioreq.h` | Numeric big-endian longs for addresses, counts and handles | Separate pointer/offset/handle fields with identical wire layouts |
| `Ctx`, `Regs`, `RegFrame`, `AllRegs` | Native context pointers and undifferentiated 32-bit registers | Native runtime context, typed register façade, explicit ABI codecs |
| `tables/labels.*`, `tables/*.cpp` | Host-array labels and patched native function addresses | Guest symbol/segment relocations plus typed callable registrations |
| `runtime/glue.cpp`, `interp/chrget.cpp` | Erased native function pointers and arity-based casts | Signature-checked thunks and argument/result codecs |
| `runtime/marks.h`, `runtime/runtime.cpp` | Native Mark addresses stored in `BasSp`, `MnPile`, etc. | Native mark registry and strongly typed guest-visible tokens where opaque |
| `hal/system.h` | One raw-pointer allocator for several kinds of storage | Guest allocation APIs distinct from native allocation |
| `hal/files.h`, host HALs | Raw buffers; File and DirLock both uint32_t | Domain-typed spans and distinct resource handle tags |
| Native stack buffers and callback records | Addresses of native locals escape into guest fields | Guest scratch frames or native handle registries, with explicit lifetime rules |

Examples requiring particular care: low-page vectors including ExecBase at 4;
24-bit chip addresses; BPTR/BADDR; nested runtime recovery; Ctx saved in menu
state; SDL InputEvent addresses; native font/screen/task/library structures;
IO requests; string literals; editor temporary strings; interrupt callbacks;
and callable metadata read just before an entry in `interp/chrget.cpp`.

## 2. Public type system and hard prohibitions

Use distinct class templates, not integer/pointer aliases. Suggested names are
provisional; their contracts are the deliverable:

| Type | Meaning |
|---|---|
| `GuestAddress32` | A logical byte address in the AMOS bus/address space |
| `GuestPtr<T>` | A typed, nullable guest address, never a native T* |
| `GuestSpan<T>`, `GuestCString` | A bounded guest range/string, retaining guest pointer types |
| `GuestRef<T>` | A checked view bound to a memory instance and access permissions |
| `HostPtr<T>`, `HostSpan<T>` | Native storage in project-facing platform APIs |
| `GuestOffset<Anchor>`, `GuestByteDelta` | Relative addressing and address arithmetic with stated semantics |
| `GuestBPtr<T>` | A real Amiga word-addressed pointer; distinct from byte pointers |
| `GuestEntry<Signature>` | A guest callable slot of a declared signature |
| `HostFunction<Signature>` | An actual native function pointer of that signature |
| `Handle<Tag>` | A non-address token such as MarkHandle, FileHandle, LockHandle or HostTaskHandle |
| `GuestWord32` | Raw bits at an explicitly untyped architectural/file/BASIC boundary |

Keep GuestPtr as a small 32-bit runtime value initially. Its separate wire field
is also four bytes but is endian-coded; equal sizes do not make the two types
interchangeable. Bound GuestRef/borrow objects can carry native memory-instance,
extent, permission and lifetime information without enlarging guest records.

Rules:

- No conversion operator between GuestPtr and HostPtr, T*, void*, uintptr_t or
  an integer. No cross-domain constructors, including explicit constructors.
  Null construction is explicit through the appropriate type/default value.
  An explicit boolean test may be provided. Same-domain mutable-to-const
  conversion is allowed; arbitrary retyping requires a named, constrained view.
- No public `from_host`, generic `address`, raw-integer factory, `native()`, or
  pointer-returning `get()` on a guest type. Wire/BASIC decoding is confined to
  named codecs with runtime validation appropriate to that boundary.
- No public GuestPtr `operator->` returning T*. A proxy whose arrow eventually
  returns T* also leaks the domain. No implicit conversion from GuestRef to T&;
  no guest iterator whose dereference exposes a native reference. GuestSpan
  does not expose a native `.data()`; a data accessor may return GuestPtr.
- Ordinary public guest operations take GuestPtr/GuestSpan/GuestRef. Native
  platform operations take HostPtr/HostSpan or distinct native handle types.
  Raw SDK pointers exist only inside trusted platform/translation implementations.
- Do not allow a common implicit void-pointer or integer representation to erase
  the distinction. Guest entry, data, BPTR, file/lock and native mark types must
  not silently substitute for one another either.
- Never make `GuestPtr<T>` an alias of T* or uint32_t on the Amiga or identity
  backend. Compiler enforcement must not depend on the selected allocator.

Proposed contract examples (not an implemented API):

```cpp
void consume_guest(GuestSpan<const Byte>);
void consume_host(HostSpan<const Byte>);
void edit(GuestPtr<wire::Program>);
void display(HostPtr<native::Window>);

consume_guest(guest_bytes);  // OK
consume_host(host_bytes);    // OK
consume_guest(host_bytes);   // MUST fail compilation: same element type, wrong domain
consume_host(guest_bytes);   // MUST fail compilation
edit(native_window);        // MUST fail compilation
display(guest_program);      // MUST fail compilation
uint32_t n = guest_program;  // MUST fail compilation
std::memcpy(guest_bytes, host_bytes, length); // MUST fail compilation
```

The valid cross-domain operation is named and directional, for example
`copy_from_host(memory, guest_destination, host_source)`. There is no general
conversion that callers can use to bypass it.

C++ cannot prevent a determined programmer from forging object representations
with arbitrary casts/memcpy. The enforceable contract combines incompatible
public types with build-enforced restrictions on those escape hatches outside
a small trusted boundary. Runtime guest words supplied by BASIC/files also need
checks; static typing cannot establish the meaning or validity of arbitrary
program-provided bits. Do not describe this as complete memory/lifetime safety.

## 3. Guest memory and access model

The checked translation behavior below applies to hosted backends. Amiga uses
native-width, unchecked ordinary pointer/view operations and a stateless
identity adapter: **no extra code over native pointers at production settings**.
Type separation still applies. Explicit `try_*` operations remain opt-in checks.
The [migration ledger](GUEST-ADDRESSING-MIGRATION.md#amiga-zero-overhead-contract)
defines the native preconditions and required m68000 code-generation gate.


Create a GuestMemory backend that initially reserves a stable 4 GiB *virtual*
window at an arbitrary native address, then commits/maps only the RAM regions
needed. It need not consume 4 GiB of physical memory. Do not request a native
base below 4 GiB. Keep one process-wide AMOS address space initially; GuestRef
binds access to its owning memory instance. If the full reservation is unavailable,
fail clearly initially; a later sparse/page-table backend must implement the
same interfaces, not reintroduce casts.

The native base-plus-offset calculation is private to `memory/detail`. Region
metadata defines committed RAM, read-only data, callable-slot bytes, device/MMIO
ranges and holes. Validate entire ranges, overflow, permissions and bounded
strings before accessing host memory. Invalid guest accesses become defined
GuestFault/AMOS errors, not host segmentation faults. Use the host's page size
for VM operations, independently of logical guest allocation alignment.

Preserve a flat numeric guest address space and byte arithmetic. Typed cursor
advance uses guest element sizes, never sizeof(native SDK types). Define checked
byte/element offsets and differences; guest wraparound arithmetic occurs only
through explicit 32-bit ALU operations where the AMOS language requires it.
Never subtract/order native pointers from unrelated backing allocations to infer
a guest address. Byte/BPTR conversion checks alignment and shift/range rules.

### Guest pointer arithmetic is required

Guest pointers represent flat logical addresses, not opaque allocation handles.
An interior bank pointer must retain its meaning when passed through tables,
registers, wire fields or a platform adapter. For a byte-addressed bank:

```cpp
GuestPtr<Byte> bank = /* guest bank base */;
GuestPtr<Byte> payload = bank + 10; // ten guest bytes, still a guest pointer
GuestPtr<Byte> next = payload + 1;
// consume_host(payload);          // compilation error remains mandatory
```

Support `+`, `-`, `+=`, `-=`, increment/decrement and same-domain comparisons
where needed by migrated code. Byte pointers advance in bytes; typed pointers
advance by `guest_sizeof<T>`, obtained from the guest schema, never the native
representation. Reject arithmetic on void, incomplete/unsized guest types,
callable entries and handles. Provide a named byte-offset operation for explicit
byte addressing of a typed object. Pointer differences return a signed guest
distance type, never a pointer or an implicitly usable address integer; element
differences require compatible types and divisible byte distances.

Compute offsets, multiplication and subtraction in a sufficiently wide checked
intermediate. No silent 32-bit wrap, truncation or saturation: named `try_advance`
operations return a fault result; operator forms use the established guest-fault
boundary, including its nonlocal-jump cleanup policy, without requiring C++
exceptions. Valid one-past cursors are allowed but cannot be dereferenced;
advancing past the representable address space faults. Null object-pointer
arithmetic faults. Explicit bus/ALU operations retain separately specified
address-zero and wraparound semantics.

Arithmetic itself does not dereference memory or require a single allocation's
provenance: AMOS permits numeric addresses and independently formed interior
pointers. GuestSpan cursors additionally enforce their retained range. Every
actual access checks the resulting address, full byte extent and permissions;
an address in a hole is not made accessible by computing it successfully.
Ordering/difference refer to the single logical guest address space, without
invoking native C++ pointer arithmetic across allocations.

The mapping invariant for ordinary contiguous RAM is:

```text
translate(bank + byte_delta) == native_arena_base + guest_address(bank) + byte_delta
```

This equality describes private implementation addresses, not a public pointer
conversion. An allocator must preserve each live guest address and its contents;
resizing/moving a bank follows existing AMOS semantics explicitly. Arithmetic
must never create a new mapping or treat `bank + 10` as an unregistered handle.
A future sparse backend resolves the resulting logical address afresh, including
cross-page accesses, and preserves the same byte-level behavior without assuming
adjacent native backing pages. MMIO uses bus handlers instead of this RAM formula.

Add tests for bank-base-plus-ten read/write and persistence through wire fields
and register codecs; negative offsets and element scaling; one-past versus
access; overflow/underflow; offsets across guest allocations, pages and holes;
and sparse backing with nonadjacent native pages. Run equivalent fixtures on
typed identity and translated backends with native backing above 4 GiB. Include
compile-fail cases proving arithmetic results cannot enter host APIs, and that
host/guest pointer subtraction and wrong-domain offsets are rejected.

Keep fixed guest semantics independently of native placement: low vectors,
chip memory's 24-bit range and custom registers. Distinguish nullable object
pointers from explicit bus access to address zero; characterize the current
PEEK/LEEK behavior before specifying that case. Device ranges use bus handlers,
not a raw pointer into a reservation. An entry slot may expose defined readable
metadata bytes without becoming native executable code.

The allocator works in guest addresses and retains current partial-free,
AllocAbs, bank-shrink and memory-flag semantics. Page commitment is a separate
layer: a 16 KiB host page must not turn an eight-byte guest allocation into a
16 KiB allocation. Debug allocation metadata may detect bad spans/UAF, but a
four-byte guest address alone cannot distinguish an old allocation from a new
one at the same address. Do not promise generation safety for ordinary RAM.

## 4. Wire records and checked guest views

Introduce `wire::PtrField<T>`, `wire::EntryField<Sig>`, `wire::HandleField<Tag>`
and typed relative-offset fields, alongside existing numeric endian scalars.
Their loads return the corresponding strong value, not uint32_t. Their stores
accept only that value. No generic scalar assignment should accept a HostPtr.

A representative access is a checked field view such as
`memory.ref(program).field<&wire::Program::Prg_StBas>().try_load()`, returning a
result containing GuestPtr<Byte>. Exact ergonomics can improve after the type
contract is tested. Generate field metadata/accessors where needed rather than
exposing a mutable native struct reference for convenience.

Treat guest storage as bytes. Use endian-aware scalar loads/stores and memcpy
for alignment-safe access; do not cast arbitrary guest bytes to a live C++ T&
and assume alignment/object lifetime. The trusted Amiga adapter uses the
original native-aligned scalar accesses; it never exports a native reference
through a guest view. Immutable snapshots may be copied to
native objects with typed fields, but they are not automatically live views
and writes require explicit writeback. The native address of a snapshot is not
a guest pointer. Volatile/event/device fields need explicit access semantics.

Every guest-visible record needs a schema/traits definition: fixed size,
packing, offsets, endian interpretation and allowed field categories. Forbid
native pointers/references, size_t, uintptr_t, virtual functions, C++ containers
and native handles with pointer representations. Merely asserting
`is_trivially_copyable` is insufficient: raw pointers are trivially copyable too.
Use generated/registered field descriptions with recursive whitelist validation.

Do not dump native SDK records into guest RAM. Define guest mirrors for the
NDK objects the interpreter/programs can observe (Task, Library, ports/messages,
Interrupt, Screen, BitMap, TextFont, DOS records), following the existing
`core/ioreq.h` approach. Platform SDK objects remain native, normally packed,
and are connected through adapters/registries.

Public strong value/handle/view types need explicit layout/alignment declarations
consistent between `-fpack-struct=2` core files and normally compiled native
files. Wire declarations separately preserve the original ABI. Check public
value-type size/alignment on both sides of that boundary to avoid an ODR/ABI
mismatch. Do not infer a wire field's layout from native pointer alignment or
from the runtime wrapper's representation. Retain every existing guest offsetof,
size and serialized-format assertion on all targets.

## 5. Registers, symbols, callable code and native handles

### Registers and ABI codecs

Do not assume A registers always contain addresses or D registers never do.
Retain the architectural 32-bit register-file representation with a sealed
GuestWord32 type and explicit scalar/address/entry/handle operations. Add typed
call-site façades for each routine's parameter/result contract. Strong pointers
must never decay to raw integers just to fit an old register assignment.

Raw-word decoding is permitted only in the interpreter/ABI codecs. BASIC's
numeric addresses (PEEK, POKE, VarPtr, bank addresses, etc.) are guest addresses;
validate them against the guest bus. They never describe native process memory.
Context registers representing program, token and value stacks become typed
guest cursors. Scheduler state, native stacks and jmp_buf remain host-only.

### Data and entry symbols

Replace `uint8_t *const` labels with typed data/code symbols. A symbol identifies
a guest segment/offset or callable slot; resolve it only after bootstrap binds
the segments. Copy immutable/mutable AMOS table images into guest storage and
apply a relocation manifest whose kinds distinguish guest data, relative offset,
entry and opaque handle. Preserve interior offsets, aliasing and metadata reads
around entry slots. Never scan all numeric longs and guess which are addresses.
Host arrays/string literals are source data for copying/registration, not values
that can be encoded with a generic address(host_pointer) helper.

### Typed dispatch and callback adapters

Replace the erased fn plus integer-arity dispatcher in `runtime/glue.cpp` and
`interp/chrget.cpp`. Generate signature-specific thunks from actual declarations
and guest ABI metadata. A binding must fail compilation when its parameter or
result domain does not match; a guest pointer result cannot be treated as a
native pointer or a native return value silently narrowed to 32 bits.

Native implementations remain real typed function pointers in a host registry.
GuestEntry<Signature> resolves to a typed thunk. A uniform internal dispatcher
may call a verified register-frame thunk, but must not cast void* to a guessed
function prototype. Test signed/unsigned narrow values, floats/doubles, zero to
ten parameters, pointer arguments/results and the split-register return rules.

Preserve existing Amiga external C/assembler entry ABIs with generated boundary
veneers. Do not assume changing a uint32_t parameter/return to a C++ wrapper
preserves every target's ABI. Public C++ callers use typed façades; raw wire-word
veneer declarations stay private to the ABI bridge.

### Native objects and tokens

Use separately tagged registries for file handles, locks, native callbacks,
font/render state, tasks and runtime marks. Use checked generation/lifetime
management for native handle tokens and define exhaustion rather than silently
reusing a live/stale generation. No generic registry token converts to GuestPtr.
If the guest needs to read a resource record, provide a guest mirror containing
only valid guest fields, not the native object or its address.

Classify `BasSp`, `MnPile`, `Tt_EvPile` and saved Ctx references individually.
Only fields proven opaque become mark/task/context handles. A field used in
guest address arithmetic needs a genuine guest representation, not a handle
that merely happens to occupy four bytes. Mark objects/jmp_buf and native return
addresses stay at arbitrary native addresses; native assembly stack reset on
Amiga gets its own veneer rather than imposing native Mark semantics on it.

## 6. HAL boundaries, scratch storage and lifetime

Split guest allocation from native allocation first. Then migrate files,
graphics, audio, devices, workbench/input and callbacks to explicit domains.
For example, a guest file read takes a tagged guest file token plus a
GuestSpan<Byte>; its native adapter calls the OS using a native file object and
HostSpan<Byte>. The initial boundary can copy via bounded bounce buffers.

A private zero-copy optimization may borrow/pin RAM for a known native leaf
operation, but must not publish T* or HostSpan aliases of guest storage to
ordinary callers. No arbitrary public mapping lambda receiving void*/T*.
Reentrant/asynchronous SDK calls require owned copies or an explicit retained
mapping/completion policy; no stack temporary may become a deferred pointer.

Replace native local-address escapes with two mechanisms:

- Guest-visible temporary strings/records/argument blocks live in GuestFrame
  scratch allocations and have explicit retention/copy-out rules.
- Purely native state becomes a tagged host registry entry, never guest data.

Do not rely on RAII destructors across setjmp/longjmp. Associate guest scratch,
borrow leases and native temporary registrations with an explicit per-task
cleanup stack/watermark recorded in runtime marks. Unwinding releases/cancels
the corresponding resources before longjmp. A callback must not use a freed
scratch frame; asynchronous operations must retain ownership or complete/cancel
before unwind releases their buffers.

Validate mark lifetime, memory instance, task identity and permitted destination.
Do not jump into a returned frame or an unrelated suspended task without an
explicit scheduler transition. Keep capture inside a live function and mutable
recovery state outside the capture frame where required, preserving the recent
local-jump fix. Native SDK callbacks must establish a guest error boundary so a
guest jump cannot escape through Cocoa/SDL/OS frames and skip their cleanup.
Fault-return APIs must work without C++ exceptions; use a C++20-compatible result
type. Any must-fault convenience layer must honor the same cleanup rules.

## 7. Compiler enforcement and the trusted boundary

Proposed ownership boundaries:

- `core/guest_types.h`: public domain types, no native address decoder.
- `core/guest_layouts`/schema: wire fields and validated layout metadata.
- `memory/guest_memory.h`: bounded access/copy/allocation APIs using strong types.
- `memory/detail` and `platform/*/detail`: the small audited implementation that
  computes native addresses or unwraps SDK objects.
- `abi/*`: raw guest-word codecs, typed call façades and generated veneers.
- `runtime/host_handles.*`: tagged native registries and cleanup records.

Keep unsafe constructors/helpers private and their headers unavailable to
ordinary guest code. Add compiler/AST checks for pointer/integer casts, C-style
casts, bit_cast/memcpy forging of address wrappers, native SDK pointer fields
inside wire records, and inclusion of private mapping headers outside the
allowlist. Diagnostic violations fail the build; source grep is only a quick
supplement, not the final enforcement mechanism.

Compile-fail fixtures are required from the first type-only change. First prove
the headers and valid examples compile, then require each bad translation unit
to fail for its intended argument/domain error. Test both directions, same-T
GuestPtr/HostPtr, raw T*/void*, spans, null/count confusion, integer laundering,
wrong handle tag, wrong callable signature, const removal and accidental SDK
calls. Add `is_convertible`/`is_invocable`/concept checks as fast supplementary
tests. A cast that compiles only because its trusted escape hatch is public is
a design failure, not something to document as caller responsibility.

## 8. Staged implementation and merge gates

This is a multi-stage refactor across hundreds of files, not a global replacement
of ptr() followed by casts to make it compile. Each stage must build/test, and
converted modules must not regain implicit conversions to call legacy code.

1. **Inventory and type contract.** Produce a field/signature/stack-escape ledger
   with each use categorized as RAM, entry, relative offset, handle or native
   state. Add public strong types, layout contracts, compile-fail fixtures and
   boundary lint infrastructure in a separate tested library. No runtime switch.
   Gate: every required wrong-domain call is rejected by GCC/Clang.
2. **Memory and wire substrate.** Implement translated RAM at high native
   addresses, permission/range checks, guest allocator, bus special regions and
   typed wire views. Add an identity backend using the same strong interfaces
   for reference/Amiga integration. Test endian/unaligned access, offsets,
   partial free, null/low page, BPTR and 16 KiB commitment independently.
3. **Bootstrap and dispatch.** Register table segments/symbols, apply typed
   relocations, introduce GuestWord register codecs and signature-checked thunks.
   Migrate a small representative set of entries, including a pointer result,
   callback and scalar operation. Gate: wrong-domain binding fails compilation;
   correct calls produce the same guest results with high native code/data.
4. **First vertical slice.** Migrate bank/string/file primitives end-to-end,
   including guest-visible wire records, file tokens, allocation and buffers.
   Run fixtures against typed identity and translated backends. Keep the legacy
   engine usable as a regression reference until integration is complete.
5. **Core interpreter and recovery.** Migrate token/source cursors, values,
   library calls, Ctx, guest stacks, marks, scratch frames and task/error cleanup.
   Gate: high-address headless BASIC with nested errors, PRUN/menu returns,
   callbacks and allocation failures; no native stack address in guest storage.
6. **Editor and remaining HALs.** Migrate editor/monitor, file-selector tasks,
   input/interrupts, fonts/screens/graphics/audio and OS interactions. Exercise
   guest fields accessed by programs and ensure transient SDK state is native.
7. **Whole-program strict mode.** Remove legacy conversion helpers and every
   bridge exemption; replace old raw-pointer public signatures. Gate: all
   modules use the domain types, all handles/entries are tagged, no native
   function-address patching remains, and forbidden casts fail static analysis.
8. **High-address default and native ARM.** Run translated Linux with PIE/high
   RAM, then Windows high placement, Intel Mac and Apple Silicon. Only after
   high-address tests pass remove low-address link/stack restrictions and the
   ARM startup guard for the translated backend. Qualify final signing/DMGs.

During stages 1-6, unconverted legacy code is explicitly outside the complete
safety claim. If integration needs a legacy bridge, isolate it in a named private
translation unit with a finite migration ledger and no public raw-pointer helper;
remove it at stage 7. Do not scatter `legacy_ptr()` calls across converted code,
or make type aliases switch between pointer and integer representations under
an experimental flag. Complete compiler enforcement is the stage-7 gate.

Amiga must retain its physical memory/OS behavior and native-pointer performance.
Its trusted identity adapter performs native accesses, without translation or
automatic checks; shared code still uses distinct strong guest/host types.
Every converted hot path must pass a no-extra-code comparison against its native
implementation at production `-Os`, including call/return boundaries. Preserve binary layouts and C/assembly veneers, then compare
emulator behavior with reference binaries. Each staged merge retains the Linux
suite and Windows/Amiga builds; once the typed engine is integrated, typed
identity and translated configurations both become mandatory regression jobs.

## 9. Validation and completion criteria

- Negative compile tests and boundary AST checks pass on GCC/Clang and remain
  active with optimization; public type ABI is identical across packed/native
  translation units. All wire sizes/offsets remain valid on each target.
- Unit/property tests cover high host bases, complete-range overflow, read-only
  memory/holes/MMIO, endian loads, odd guest addresses, BPTR, integer ABI decoding,
  entry aliases/relocations, registry exhaustion/stale handles and bad strings.
- Run the existing 85 host text/interpreter regressions and GUI fixtures, adding
  focused monitor, file-selector, error-recovery, clipboard, task and async-I/O
  tests. Compare serialized .AMOS/bank/config bytes and guest semantics. Address
  values that were incidental native placement may differ; characterize such
  tests rather than silently changing format or behavior goldens.
- Force backing RAM, native tables, functions and ordinary stacks above 4 GiB
  in Linux tests, rather than merely hoping ASLR does it. Remove native host
  pointer truncation from all bootstrap/dispatch paths. Use sanitizers on the
  translated high-address backend where supported.
- Run Amiga emulator/reference tests, Windows and Intel Mac tests, and native
  Apple Silicon debug/optimized tests with the measured 16 KiB page size. Validate
  integer/SIMD/FP context state and guest error boundaries under final signing.
- Benchmark interpreter dispatch and graphics only after correctness. Hoist
  checks through bounded guest cursors/leases or generated accessors; do not
  restore raw-pointer conversions to recover performance.

Completion means a working high-address runtime with compiler-enforced domain
separation throughout the application, preserved guest ABIs/formats, and native
arm64 runtime validation. A successful cross-link, four-byte wrapper, or bounded
allocator alone does not meet that definition.

## First implementation change to attempt

The initial change implements the strong type/header contracts and compile-fail harness,
plus the inventory ledger and a standalone high-address GuestMemory testbed.
Tests use the same element type in positive/negative host/guest API tests so rejection
cannot be attributed merely to different pointee classes. The initial GuestRef/field
access API is exercised by a small linked-list/string example before migrating
registers or hundreds of call sites. This gives an early, reviewable proof of
the user's compiler-error requirement without destabilizing the current engines.

## References

- [C++ conversion functions](https://eel.is/c++draft/class.conv.fct): explicit versus implicit conversion rules.
- [C++ object lifetime](https://eel.is/c++draft/basic.life): alignment/lifetime requirements at memory access boundaries.
- [Custom clang-tidy checks](https://clang.llvm.org/extra/clang-tidy/Contributing.html): infrastructure for build-enforced escape-hatch checks.
