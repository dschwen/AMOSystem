# Guest addressing migration ledger

Runtime migration is in progress, and **is not complete**. Production dispatch,
allocation bookkeeping and recovery now use the new typed machinery described
below. The interpreter still uses `core/amos.h`'s legacy `ptr/address` API for
ordinary data, and still requires low native code/data/stack placement. Native
Apple Silicon execution remains unsupported.
See [the design](GUEST-ADDRESSING.md) for the complete migration and merge gates.

## Implemented substrate

- `core/guest_types.h`: distinct GuestAddress32, GuestPtr, HostPtr, GuestSpan,
  HostSpan, byte deltas and signed typed distances. No native pointer exposure
  from guest values, no integer decay, no raw pointer constructors. Const
  qualification can be added but not removed. Guest layout determines stride.
- `abi/guest_address_codec.h`: explicitly named 32-bit architectural/wire codec.
  Decoding raw bits does not assert that the address is mapped; access checks
  happen in Memory. This codec never accepts native pointers.
- `PointerField<T>`: exactly four big-endian bytes; load/store retain GuestPtr.
- `memory/guest_memory.h`, `host/guest_memory.cpp`: a private high native 4 GiB
  virtual reservation, exact logical RAM regions, bounded directional copies,
  byte/big-endian long access, whole-range validation and logical permissions.
  POSIX and Windows backends; actual host page size controls VM commitment.
- `memory/guest_view.h`: checked, memory-bound record views and schema field
  descriptors. Pointer fields load/store only GuestPtr; scalar fields use
  endian-safe accesses. Field selection preserves record constness and exposes
  neither native references nor native pointers. A linked-list/string fixture
  traverses two guest records and an interior bank pointer through these views.
- `guest-memory-probe` in the Mac probe bundle: high backing, bank-plus-ten,
  endian access and a live typed pointer field. A pass qualifies this substrate
  only; the old address probe still fails for the legacy runtime on arm64.
- `make test-guest-addressing`: wrong-domain compilation failures, packed/native
  ABI and Amiga header checks, plus high-address debug/optimized runtime tests.

Arithmetic is not an allocation registry: base plus ten maps to the same bank's
byte ten. Named `try_advance` and `try_offset_bytes` return faults; operator forms
on translated hosts currently terminate on invalid arithmetic. On Amiga the
ordinary operators have native preconditions as described below. The standalone
host failure hook must be replaced by the per-task AMOS recovery adapter **before** interpreter integration.
No AMOS longjmp/cleanup policy is implemented by this first substrate.

The memory substrate is not yet a bus, sparse backend, general wire-schema
engine or complete static-analysis enforcement. Its mapped regions currently
have a fixed 128-entry limit and live until Memory destruction. Partial free and
AllocAbs are implemented by the separate logical RangeAllocator, without
unmapping RAM or discarding its contents. GuestSpan is a range description: use subspan for checked
slicing, and Memory validates all actual accesses. There is no public native
mapping/borrow facility.

## Integrated runtime changes

- All 1,951 implemented glue callbacks use native signature-specific thunks on
  hosts. `tools/sync_runtime_bindings.py` emits them into each defining TU so the
  compiler sees the actual function declaration. Metadata drift, wrong argument
  counts/result widths, wrong callback families and unsupported codecs fail checks.
  RegFrame pointer and reference signatures are adapted explicitly; three-word
  float records are copied through their actual C++ type. There are no host
  arity-based function casts in the glue dispatcher or ChrGet fast path.
- Direct system/screen vectors use the same checked declarations. Obsolete
  host `void function()` guesses are excluded. Host tables do not erase native
  callback pointers into `void*`, and 748 LibCall metadata slots no longer contain
  truncated native function addresses. Amiga emits its original metadata/calls.
- `RangeAllocator` is used by the production host allocator. Free ranges carry
  GuestPtr addresses, support first-fit, eight-byte size rounding, partial free,
  merges, AllocAbs and exact top-of-address-space bounds. Its metadata is native
  malloc/realloc storage; RAM contents survive free/reallocation. The HAL backing
  boundary still uses the old low-address arenas pending data-call-site migration.
- `BasSp`, `MnPile`, `Tt_EvPile` and menu `MnRA3` are four-byte typed wire fields,
  with unchanged sizes and offsets. Native recovery uses generational MarkHandle
  tokens with capture/lifetime/task checks, not native Mark addresses. Menu
  register snapshots have a separate ContextHandle type and own their Ctx values.
- Normal return and nonlocal jumps explicitly release scoped context snapshots
  through capture watermarks; no RAII destructor is relied on across longjmp.
  Handles are retired rather than reused after generation exhaustion. Native
  marks and snapshots can reside above 4 GiB; their wire tokens remain 32-bit.
- Native dispatch indexing uses host allocation rather than guest RAM. Native
  register/context ABI is stable across packed/nonpacked TUs, and OS ucontext
  layouts/coroutine storage retain native packing/alignment.

### Still required before a high-address runtime

1. Register/copy static guest data segments and symbols, distinguish callable
   slots from data addresses, and replace native-address-based table relocations.
2. Migrate ordinary guest data pointers/cursors, register façades, guest records,
   array/field access and bank/string/file public APIs; remove the finite raw-data
   argument bridge in `runtime/detail/legacy_call_codec.h`.
3. Replace remaining native temporaries/static objects escaping as guest words
   with guest scratch storage or typed native handles and explicit cleanup.
4. Separate platform native objects from guest mirrors and migrate file/lock/task
   handles, graphics/fonts, input/audio and asynchronous buffer lifetimes.
5. Switch production backing to high GuestMemory, remove all legacy bridges,
   enforce whole-program boundary checks, qualify platform runtime behavior and
   only then remove low-address flags/guards.

The isolated high-address memory testbed is not a substitute for these remaining
steps. No new flag bypasses the ARM startup guard or advertises a translated
release. `GuestPtr`/HostPtr/handle default initialization is trivial; use `{}` for
null/zero, matching the difference between native default/value initialization.

## Amiga zero-overhead contract

On real `__mc68000__` targets the two pointer domains remain distinct C++ types,
but ordinary operations use the identity backend. Neither GuestPtr nor HostPtr
has implicit native/integer conversions. Both have the native pointer's size
and alignment. GuestRef holds just one native-width guest address, with no
memory-owner pointer. Offsets, distances and span counts use native 32-bit
widths; spans occupy a pointer plus a native count. Wire pointer fields are
native big-endian longs with Amiga alignment and the original four-byte size.

Ordinary arithmetic, field selection, scalar/pointer access, copying and span
slicing introduce no translation, region lookup, allocation or automatic
null/overflow/bounds checks on Amiga. They inherit native pointer validity,
alignment, range and lifetime preconditions; copies require valid matching
ranges. Numeric zero and hardware accesses retain native machine behavior.
Explicit `try_*` helpers are opt-in checked operations and may cost code.
Translated host backends retain their checks. Amiga VM reserve/map operations
are deleted so shared startup cannot accidentally allocate a translated arena.

`tests/guest/amiga_codegen.cpp` compares actual m68000 object disassembly and
relocations against raw-pointer implementations at production `-Os` and `-O2`.
It covers arithmetic, distances, comparisons, loads/stores, record fields, wire
pointers, spans, memcpy, argument/return ABI, external typed calls and a byte
cursor loop. At -Os the tested instructions match exactly except commutative
operand order in slicing. At -O2 the hot loop is identical and its setup is
smaller; other operations retain the same native code. No added helpers or
calls are permitted. `make test-guest-addressing` runs this gate when the Amiga
toolchain is installed, and runs the domain rejection tests with that compiler.

Carry this code-generation gate into every converted Amiga hot path. Trivial
storage size alone is insufficient proof. In particular, forwarding a class
return through an integer-return veneer can inhibit GCC sibling-call elimination
even when the physical return registers match. Keep internal pointer APIs typed
end-to-end and test each actual C/assembly boundary before migration; do not
accept additional bridging instructions as an unavoidable wrapper cost.
The existing production Amiga engine is still unchanged by this substrate.

## Classification ledger and next conversion boundaries

These are semantic categories, not permission to bulk-replace casts. Populate
individual field/entry signatures as each module is converted; unclassified
legacy uses remain outside the compiler-enforced guarantee.

| Boundary | Representation / audit needed | Migration gate |
|---|---|---|
| `core/amos.h`: `ptr`, `address`, `Ctx`, `idata/wdata` | Raw host pointers mixed with architectural words | Typed contexts, wire views and explicit register codecs throughout callers |
| `core/layout.h`, `core/ioreq.h` | Distinguish addresses, sizes, offsets, opaque tokens and callable fields | Byte-identical schema layouts; guest fields never hold HostPtr |
| `tables/labels.*`, `tables/*.cpp` | Guest data labels versus native entries; relocations and metadata preceding entries | Registered guest segments and typed relocations, no native function pointer truncation |
| `runtime/glue.cpp`, `interp/chrget.cpp` | Erased signatures and arity-based native function casts | Signature-specific thunks, argument/result codecs and negative binding tests |
| `host/hal_system.cpp`, `hal/system.h` | Guest RAM allocations versus native OS objects, chip addressing, partial free | Guest allocator, AllocAbs/free/shrink and bus semantics preserved |
| `core/source.cpp`, `lib/banks.cpp`, bank/string entry points | Interior guest cursors and numeric BASIC addresses | End-to-end bank/string/file vertical slice on identity and translated memory |
| `hal/files.h`, `host/hal_files.cpp` | File/lock tokens versus buffers and native objects | Tagged resource handles and directional span adapters |
| `runtime/marks.h`, `runtime/runtime.cpp`, `runtime/monitor.cpp` | Marks stored as 32-bit addresses; classify opaque versus arithmetic uses | Tagged mark registry, task/lifetime checks and cleanup watermarks |
| Editor status/screen temporaries, SDL InputEvent records | Native local addresses escaping to guest words | Guest scratch frames or native handles with explicit lifetimes |
| `host/tasks.cpp`, `host/context.cpp` | Native task contexts versus guest stacks/state | No native stack address in guest records; recovery/context validation |
| Native fonts/screens/IO requests/task/library structures | Mixed SDK and guest-visible layouts | Guest mirrors, native handles and typed platform boundaries |

The linked-list/string example exercises the field-view API; the live allocator,
callback bindings and native recovery/snapshot state are now integrated. Static
symbols, ordinary data-pointer call sites and platform mirrors remain the
critical blockers. A high reservation cannot safely replace legacy backing while
those callers still encode native pointers as architectural words.

## Workaround removal checklist

No existing low-address workaround is redundant yet. The new subsystem adds no
low-address allocator flags, image-placement flags or public pointer escape hatch. Remove each at the gate
below, rather than hiding it behind another experimental flag.

| Existing restriction | Removal condition |
|---|---|
| Linux `-no-pie` in Makefile | All static data/code references use guest symbols or typed native registries; translated high-PIE runtime regressions pass |
| Intel Mac `-no_pie,-pagezero_size,0x1000` in Makefile and `tools/macos_cross.py` | Same code/data migration and native Intel Mac validation |
| Windows low image base / disabled ASLR and image gate | All data/code pointers translated; high-image/ASLR Windows runtime validation |
| ARM startup address-model guard in `host/main.cpp` | Entire translated engine runs correctly on native Apple Silicon, including task/error paths |
| `MAP_32BIT`, low hints/chip native placement in `host/hal_system.cpp` | Guest allocator preserves logical chip/fast ranges without requiring matching native addresses |
| Low native interpreter/task stacks | Native marks, scratch frames, contexts and callbacks no longer escape as guest words |
| Low-page special native array in legacy `ptr` | Explicit translated low-page/bus behavior, including null versus numeric address zero |
| Legacy `ptr/address`, raw-pointer public APIs and bridge exemptions | Whole-program strict mode passes compiler rejection and AST checks |
| Direct-pointer address probe as release gate | Translated runtime probes replace that obsolete gate; keep old probe as historical/reference tooling if useful |

Keep guest 32-bit widths, guest endian/packing, logical chip addresses, platform
SDK native packing and validated local-jump/context fixes. Translation does not
make those constraints redundant. Do not remove standard/native jump handling,
SDL platform code or VM page-size handling as part of address cleanup.

## Validation of this change

GCC and Clang pass positive/negative compilation checks and the standalone
high-address tests at O0/O2. MinGW passes the type contract and builds AMOS;
Amiga passes the packed header checks and its existing build. Linux AMOS reports
85 passed, zero failed, 54 skipped in the host text regression suite. The translated memory
and field-view testbed also passes AddressSanitizer/UndefinedBehaviorSanitizer.
Apple Silicon AMOS and the new memory probe cross-link; native execution of the
new probe remains pending. CI runs the isolated contract with GCC and Clang.

Latest integrated validation: 85 host text tests passed (54 skipped), and five
menu/editor GUI fixtures passed. GCC/Clang contract/runtime tests pass; Amiga
code-generation checks show no extra wrapper instructions at production -Os.
Windows and Amiga builds pass, and the arm64 executable cross-links with its
startup guard retained. Recovery/snapshot cleanup and allocator model tests pass
AddressSanitizer/UndefinedBehaviorSanitizer. These results do not qualify the
remaining legacy data-pointer paths or establish native arm64 runtime support.
