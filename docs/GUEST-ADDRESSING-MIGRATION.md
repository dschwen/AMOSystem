# Guest addressing migration ledger

Runtime migration is in progress, and **is not complete**. Production dispatch,
allocation bookkeeping and recovery now use the new typed machinery described
below. The interpreter still uses `core/amos.h`'s legacy `ptr/address` API for
ordinary data, and still requires low native code/data/stack placement. Native
Apple Silicon execution remains unsupported.
See [the design](GUEST-ADDRESSING.md) for the complete migration and merge gates.

## Biased runtime (`AMOS_GUEST_BIASED`, the path to arm64)

A second production mode, next to the identity mapping, that needs none of the per-site
conversions below to *run*: guest address `a` is `amos_guest_base + a`, with the guest memory
(4 GiB reserved `PROT_NONE`, the first 1 GiB read/write: chip memory at 1 MB, fast memory from
4 MB) mapped wherever the OS puts it. `core/guest_bias.h` is the one place that knows
(`bias::to_native`, `to_guest`, `contains`); `core/amos.h`'s `ptr<T>`/`address()`, the live
context cursors, `ChipPtr` and the legacy dispatch adapters use it, so the identity mapping is
the same code with a base of 0. Built with `make host HOST_BIASED=1` (the default for
`HOST_TARGET_ARCH=arm64`).

- **Static image in guest memory.** `prepare_static_symbols()` copies each table segment into the
  guest memory (chip segments into chip memory), applies the 2,206 relocations there and binds
  the symbols to the copies. The 128 data-label uses (`ed_header`, `w_req_pal`, `music_data`...)
  are `AMOS_DATA(label)`: the label's place in the table on the Amiga, in the loaded image
  elsewhere. Code dispatch resolves through the same bindings, so entries are guest addresses of
  the image.
- **Native pointers as handle windows.** A native address that is not guest memory (a static or
  stack object, a callback) and is asked for as a guest address gets a window of the handle
  region (0xC0000000 up): the 1 MB of native address space around it. `to_native` maps it back,
  so equality, ordering and increments within a MB behave as a flat 32-bit address did
  (`context.cpp`; `AMOS_ESCAPE_LOG=1` lists every place that makes one). This is the bridge for
  the "native temporaries and objects that escape as guest words" items; each place on that list
  is still to be given real guest storage, and the handle region then disappears.
- **Bugs it exposed.** Arithmetic on a pointer with a 32-bit offset that was meant to wrap
  (`ptr(plane) + uint32(-2)` in `w/bobs.cpp`) only worked because the identity mapping truncated
  the result; `-1`/`0xFFFFFFFF` markers are native addresses that fault when used.
- **No native address becomes a guest word any more** in the host suite (`AMOS_ESCAPE_LOG=<path>`
  lists the sites; it is empty): the system's own objects (the task, devices, the input handler,
  IO requests and events, the ioports data zone) are allocated in guest memory (`guest_object`),
  native text and constants the guest reads are copied there (`guest_text`, `guest_constant`),
  and C++ VBL routines the guest holds as code addresses are marker cells (`guest_callback`,
  `callback_target`), all in `core/guest_scratch.h`. The handle windows remain only as a safety net.
- **Native Apple Silicon runs.** The crash on arm64 was the host's `-fpack-struct=2` applied to
  the system's own structures: macOS `struct stat` is larger than its packed twin, so `stat()` wrote
  past it onto the caller's stack. `src/host/native_headers.h` (forced in with `-include`) reads
  `sys/stat.h`/`dirent.h` with the platform's packing. Also: `c:assign` locks the executable via
  `_NSGetExecutablePath` (there is no `/proc/self/exe`). `.github/workflows/macos-arm64.yml` runs
  the hello program and the text tests natively: 85 of 86 pass (`insert`, which depends on
  wall-clock disk-swap timing on a loaded runner, is flaky).
- **Strings are typed.** `lib/strings.h` has `GuestString` (a guest address; `size()`, `at(i)`,
  `chars()`, `copy_to()`) and `StringBuilder` (room at HiChaine, `put`, `finish`). Library functions
  take `GuestString` instead of `const uint8_t *` (`CallArgument<GuestString>` decodes the word):
  `lib/strings.cpp` whole, `interp/text.cpp` (Left$/Mid$/Val), and the string parameters of
  files, dirs, dos, devices, arexx, keyboard, memory, screens, windows, iff, bankfiles, music and
  ioports, then the builders of textout, files, interface, keyboard, maths, screens and system.
  The legacy native-pointer `str_len/str_chars/str_alloc/str_finish` are gone.
- **The variable zone is typed.** Slots, array blocks, labels and procedure frames are
  `ContextCursor`s, not native pointers: `interp/vars.cpp`, `bits.cpp`, `sort.cpp`, `input.cpp`,
  `field.cpp`, `heap.cpp` (the string collector), `eval.cpp` (concatenate, subtract, compare),
  `control.cpp` (labels, procedure calls, For/Next with `interp_next_float`), and the verifier's
  label table and variable buffers (`verif/tables.cpp`, `buffers.cpp`, `main.cpp`). Left on native
  pointers in the verifier: the program it walks (`pc`) and its loop tables.
- **Banks are typed.** Banks, bank lists and their elements are `ContextCursor`s (`lib/banks.h`,
  `banks.cpp`, `bankfiles.cpp`, and the callers in music, amal, graphics, files, iff, interface,
  menus, screens, dialog, fsel, verif). `bank_host_view()` is the one named place a bank's memory
  goes to native code: file reads into and writes out of a bank, the pack/unpack of `ext/compact`,
  IFF loading, the menu tree copy (`tree_to_bank`/`tree_from_bank`) and the resource parser of
  `lib/interface.cpp`. Those native consumers, and the `Dialog` records that sit on list elements
  (`lib/dialog.cpp`), are the next slices.
- **Status.** All 140 host tests pass on the biased build on Linux x86-64 (guest memory at
  0x7f..., above 4 GB); the identity build and the Amiga build (134/134) are unchanged.
  `.github/workflows/macos-arm64.yml` builds and runs it on a native Apple Silicon runner.
  Struct pointers in static tables must be 8-aligned in the packed layout (arm64's linker
  refuses a misaligned relocation).

### Backing decision and program cursor (a6)

- **Backing: the arena stays.** Guest memory is one reserved 4 GiB range, committed lazily (physical
  memory is only what the guest touches; Windows reserves and commits in chunks). All translation
  stays behind `GuestMemory`/`bias::`, so a page-table backend can replace it later without touching
  typed callers. Chip and fast memory are two guest ranges (chip low, fast above), as now.
- **Live a6 is a guest cursor.** `Ctx::a6` is a `ContextCursor` (`pc()`); `Ctx` is 24 bytes on every
  build and `GuestContext` is a plain copy. Program reads go through `peek_w/peek_l/byte_at`; program
  lines, labels and procedure headers in `interp/{control,data,vars}.cpp` are cursors. Still native:
  the variable zone (`TabBas`, `VarLoc`, `LabHaut`...), the editor `Program` record
  (`identity_record`) and `source::skip_token` (cursor overload converts temporarily).

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

### Static symbol/image stage

`tools/sync_static_segments.py` now describes all 101 static segments, 300
exported labels and 2,206 relocation writes. Pointer-bearing relocations use
segment IDs/byte offsets, including relative differences and one-past labels,
with checked bounds and no native-pointer truncation. `GuestSymbol<T>`,
`EntrySymbol<Signature>` and resolved `GuestEntry<Signature>` are distinct from
both native and guest data pointers. Source bytes enter through HostSpan.

The production bootstrap resolves these symbols through a checked private
identity adapter while legacy data callers remain. The editor/monitor module
loader now receives GuestPtr parameters. Hosted modules no longer write an
assembly bridge pointer cell or retain the fake amos_task/amos_bridge globals;
Amiga retains the original bridge and literal relocation expressions.

`load_static_image` builds an independent copy in high GuestMemory, with chip
segments below the guest 24-bit limit and every relocation preflighted. Tests
load real tables from PIE sources above 4 GiB, vary logical placement, and
compare every byte against the current identity image after pointer adjustment.
`static-image-probe` now includes this real catalog in the arm64 Mac probe
bundle; native execution remains pending. The image loader is ready for subsequent caller integration; production still
uses identity placement and legacy exported raw-pointer labels alongside the
new typed `tables/guest_labels.h` catalog. This does not enable native arm64 yet.

### Guest code dispatch stage

Hosted GlueEntry metadata now stores typed EntrySymbol values for entry locations
and alias/stack targets. A native GuestDispatchIndex sorts and searches resolved
GuestEntry keys, and its cache also carries GuestEntry values. Native array
addresses no longer determine ordering, cache identity or alias resolution.
The public hosted glue_entry/glue_call and mark_jump APIs reject native pointers,
guest data pointers and entry values with another signature at compile time.

The interpreter call/evaluation/condition-code/jump bridges have typed overloads;
lib_call/lib_eval/lib_jump and known static-label dispatch/verifier calls use
these paths. lib_entry reads a code slot through EntryCodec, while lib_address
remains the temporary mixed/data-slot interface. Reading the library table still
uses identity data backing, so this does not yet migrate library-table storage.
Mark target/then continuations are typed code addresses on hosts; restart and
continue decode architectural words directly. Host library-vector arithmetic
now wraps in a 32-bit guest word before decoding, rather than forming an oversized
native pointer and relying on truncation.

At this stage raw C ABI callers and mixed native/guest interrupt handlers used
the named adapters in runtime/detail/legacy_dispatch.h; the hosted raw call
overloads are removed by the call-bridge stage below. These adapters reject unrepresentable
native addresses; they do not enable translated data access. Amiga retains raw
literal metadata, its original interpreter bridge and direct calls, without an
index or translation overhead added to those paths. The code-generation fixture
also compares GuestEntry argument/word handling against native pointer handling.

The real dispatch catalog is tested with metadata above 4 GiB and two independent
high-backed guest images. Every entry and all 763 alias/stack targets resolve to
the same native descriptor after relocation. Callback bodies in this catalog test
are explicit test stubs; production callback behavior is covered separately by
the Linux interpreter/editor regressions. This is not a full translated-runtime
execution test.

### Interpreter entry and token-table stage

`runtime/interpreter_bridge.h` owns the declarations for instruction execution,
value execution, nonlocal jumps, PRUN, editor entry and menu procedure calls.
Their hosted routine arguments, Collected body values, parameter collectors,
extension calls and native-call cache keys now carry GuestEntry. Monitor bank
switches and saved continuation words also use typed entries. These ordinary
interpreter paths no longer use legacy_glue_call/legacy_glue_entry. RoutineArg
is the original raw pointer type on Amiga, preserving its bridge ABI.

TokenTable is GuestPtr<const Byte>. Collector/table lookup parameters and cache
comparisons use guest data pointers, with scalar reads through the private
IdentityDataMemory backend. The remaining native-context table conversion is
explicit and rejects high/unrepresentable pointers; removing it requires the
context/register data migration. This backend performs identity reads and does
not yet provide the mapped-region checks of high GuestMemory.

`memory/guest_code.h` provides a memory-bound CodeReader for typed code metadata.
It exposes scalar results and faults, never a native pointer. Hosted production
uses this same reader with the temporary identity backend; tests and the Mac
memory probe use high GuestMemory. Memory now supports endian-safe 16-bit loads
and stores, validating the complete access before mutation. Tests cover code
headers, arithmetic faults, unmapped/permission boundaries and partial-write
prevention. Wrong-domain interpreter, collector and metadata arguments must fail
compilation. On Amiga the production metadata reader retains its original direct
loads; machine-code fixtures verify identical token loads and no added metadata
instructions (GCC may commute base/index registers).

### Hosted call bridge stage

The hosted amos_call/amos_call_all/amos_eval/amos_call_cc/amos_goto, entry_call and
wentry_function APIs now accept only typed code entries. Their raw-pointer
prototypes/forwarders have been removed on hosts. Amiga retains its original C
ABI implementations and literal vector addresses. Over 400 static code arguments
in editor, monitor, verifier and graphics helpers now resolve through the typed
entry catalog. Helpers that used to accept uint8_t*/void* routine arguments now
carry RoutineArg, with native pointer ABI on Amiga.

Dynamic editor function tables, monitor dispatch tables, dialog/slider/button
hooks, graphics vectors, patch hooks and routine-list code addresses decode
architectural words directly. Offset arithmetic happens in the guest word before
code decoding. The CALL path's native program cursor still crosses one explicit
checked identity adapter; native callback/interrupt dispatch still has separate
legacy adapters. Neither is a public implicit pointer conversion.

Compiler rejection tests cover the removed raw bridge overloads. Linux behavior,
Windows/Amiga builds, GCC/Clang contracts and arm64 cross-linking pass; see the
current validation summary below.

### Typed code-address wire fields

Prg_InsRet, Prg_JError, Patch_Errors, all eight ExtTests cells and the monitor's
Tt_JError now use RoutineField: exactly four big-endian bytes, with explicit
typed load/store and no integer, native pointer or guest data-pointer conversion.
All existing IData/MonData size and offset assertions remain gates. Continuation
and restart APIs carry typed entries rather than re-encoding the field as an
integer between calls.

GuestRef supports RoutineField in schema-selected mapped record views. Loads
retain code-address types, const views cannot store, and read-only mappings reject
writes without modifying bytes. These paths are tested in high GuestMemory,
including byte serialization; native field/view load/store operations are covered
by the Amiga machine-code comparison. Production IData/MonData access still uses
legacy identity records pending the context/data-view migration.

### Remaining ordinary hooks and guest snapshots

All six GoTest_* slots, Patch_ScCopy/ScFront/Menage and Button::Bt_Routines now
use RoutineField, bringing the current code-cell total to 22. Patch-table offset
selection stays explicit at the architectural code boundary. Slider::Sl_Routines
is deliberately still unconverted: dialog sliders store the native address of
an anonymous zero array there as a built-in dispatch marker. That mixed marker/
code-table representation needs an explicit replacement; treating the marker as
a real guest entry would preserve a native-address escape.

Hosted saved register snapshots now use GuestContext, a native-owned 24-byte
value with guest a3-a6 pointers and scalar d6/d7. The context registry accepts
only this type and retains generation/task/lifetime checks. GuestContext is not
constructible from the live legacy Ctx, and its pointers cannot implicitly become
native pointers or code addresses. Both menu capture sites use the private checked
capture adapter, which rejects high native cursors rather than truncating them.
This is snapshot migration, not conversion of the live interpreter context.

On Amiga GuestContext aliases the existing Ctx, and capture returns the original
context by reference. No temporary snapshot copy is introduced before the native
save-context token is made. Machine-code fixtures cover capture and saved a3
loads; hosted tests cover high logical addresses, task ownership, normal/nonlocal
cleanup and rejection of the remaining high native cursors (a3/a6).

### Live a5 data-zone cursor

Hosted Ctx::a5 is now DataZoneAddress (GuestPtr<Byte>), with explicit architectural
encode/decode at every register, task, requester and file-selector handoff. It
cannot accept native pointers, HostPtr, integers or code entries implicitly. The
boot allocator returns a typed address and crosses one checked capture_data_zone
adapter (renamed capture_cursor in the a4 stage); high native
allocations are rejected, not truncated. Saved GuestContext snapshots now copy
the typed a5 value directly, and the legacy capture adapter checks only the
remaining native cursors (only a3/a6 after the a4 stage). Task setup exports the logical a5 word directly
without re-encoding a native wdata() record reference.

At the a5 stage Ctx retained its original hosted size and offsets, including
padding after a5; the a4 stage below compacts this internal state while retaining
a stable packed/unpacked layout.
Amiga retains its original native pointer field and ABI. Machine-code fixtures
cover data-zone capture and live a5 loads/stores against native baselines.

idata()/wdata() still return legacy native record overlays. On hosts those
references now originate only in the private identity_record adapter, with null
and 4 GiB endpoint checks; this is not a mapped-memory implementation. Tests
exercise high logical a5 register values, a5-plus-ten arithmetic, context capture,
packing consistency, native-pointer rejection and a low-backed identity overlay.
The other live context cursors and mapped record conversion remain mandatory
before production can use high backing.

### Live a4 cursor

Hosted Ctx::a4 is now a guest ContextCursor. Register transfers, editor window
save/restore, token-table selection and verifier relocation cursors retain guest
addresses. The instruction/evaluation paths and hosted floating-point token-table
swaps read/write through the typed IdentityDataMemory interface; no native
pointer is obtained from a4 to read a token or append a relocation byte.
The real Amiga float-swap implementation is retained unchanged.

legacy_table_address has been removed. The former data-zone capture adapter is
now capture_cursor, shared by the two live typed cursors and explicit captures
from legacy native editor records. Editor window() uses the private identity
record adapter. These checked captures/overlays remain temporary boundaries;
this change does not translate native EditWindow storage or install high backing.

The hosted internal Ctx is now 32 bytes, with stable packed/unpacked offsets:
a3=8, a4=16, a5=20, a6=24. The redundant historical native-width gap after a5 is
no longer retained. This state is native-owned, not a persisted guest record.
The Amiga context remains its original 24-byte native layout. Machine-code
fixtures cover a4 load/store and the actual relocation-byte append helper.
Saved snapshots copy typed a4/a5 directly; the checked legacy capture now only
handles a3/a6. High logical a4 register roundtrips and rejected raw/native/code
pointer assignments are included in the contract tests.

## Remaining work checklist

Completed compiler-enforced boundaries:

- [x] Distinct guest/host pointer types and checked hosted pointer arithmetic.
- [x] Native signature-specific callbacks, guest dispatch keys and static image relocation.
- [x] Typed interpreter/collector code arguments and guest token-table parameters.
- [x] Hosted call APIs reject raw/native/data pointers; static code arguments use symbols.
- [x] Twenty-two continuation/error/extension/test/patch/button code cells and mapped views.
- [x] Guest-only saved context values and checked legacy capture.
- [x] Live hosted a4/a5 guest addresses and explicit register/task/requester transfers.
- [x] Typed a4 token/relocation access and removal of the native token-table adapter.

Required next conversions and release gates:

- [ ] Replace the mixed native marker/code-table representation in Sl_Routines,
  then migrate slider tables and OS interrupt/callback registrations.
- [ ] Migrate remaining live Ctx a3/a6, program cursors and register façades to
  guest data addresses; remove the checked native-context capture, native-cursor and
  legacy native editor-record captures.
- [ ] Replace idata/wdata/native record overlays with mapped schema views, starting
  with the interpreter's token/continuation vertical slice. Install high Memory
  behind those readers; remove identity_record, IdentityDataMemory and LegacyCodeReader.
- [ ] Convert bank/list/string/file APIs end to end, including interior cursors,
  allocation/free/AllocAbs and argument/result codecs. Remove legacy_call_codec.
- [ ] Convert remaining static data-label/source-array consumers, then install the
  translated static image and high RAM as production backing.
- [ ] Replace native local/static addresses escaping to guest words with guest
  scratch storage or typed native handles and explicit normal/nonlocal cleanup.
- [ ] Split file/lock/task/library/graphics/font/input/audio objects into native
  objects and guest mirrors, preserving asynchronous buffer lifetimes.
- [ ] Enforce whole-program pointer-boundary checks; run high-image/ASLR behavior,
  task/error cleanup and native Mac tests. Qualify the actual arm64 interpreter.
- [ ] Remove obsolete identity adapters, low-memory/image/stack flags and ARM
  startup guard only after the corresponding gates below pass; then build a DMG.

### Still required before a high-address runtime

1. Switch remaining exported-label data consumers, mixed interrupt/native
   callback callers and data-slot lookup to the typed static symbol/image catalog; finish replacing native source-array references
   before installing the translated image as production backing.
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
operand order in slicing and base/index allocation in metadata reads. At -O2 the hot loop is identical and its setup is
smaller; other operations retain the same native code. No added helpers or
calls are permitted. `make test-guest-addressing` runs this gate when the Amiga
toolchain is installed, and runs the domain rejection tests with that compiler.

Carry this code-generation gate into every converted Amiga hot path. Trivial
storage size alone is insufficient proof. In particular, forwarding a class
return through an integer-return veneer can inhibit GCC sibling-call elimination
even when the physical return registers match. Keep internal pointer APIs typed
end-to-end and test each actual C/assembly boundary before migration; do not
accept additional bridging instructions as an unavoidable wrapper cost.
Converted Amiga call sites retain the native ABI and identity operations; carry
these machine-code checks forward as additional hot paths are migrated.

## Classification ledger and next conversion boundaries

These are semantic categories, not permission to bulk-replace casts. Populate
individual field/entry signatures as each module is converted; unclassified
legacy uses remain outside the compiler-enforced guarantee.

| Boundary | Representation / audit needed | Migration gate |
|---|---|---|
| `core/amos.h`: `ptr`, `address`, `Ctx`, `idata/wdata` | a4/a5 typed; a3/a6 and identity record overlays remain | Typed remaining cursors, mapped record views and removal of identity_record |
| `core/layout.h`, `core/ioreq.h` | Recovery/context tokens and twelve code cells typed; most data/other callable fields remain | Byte-identical mapped schemas; guest fields never hold HostPtr |
| `tables/labels.*`, `tables/*.cpp` | Static catalog/relocation and code-label callers migrated; raw data-label consumers remain | Finish data consumers, then install the translated static image |
| `runtime/glue.cpp`, `interp/chrget.cpp` | Typed dispatch, collectors and thunks integrated; native context/data readers remain | Remove identity data/cursor adapters and qualify high-backed execution |
| `host/hal_system.cpp`, `hal/system.h` | Guest RAM allocations versus native OS objects, chip addressing, partial free | Guest allocator, AllocAbs/free/shrink and bus semantics preserved |
| `core/source.cpp`, `lib/banks.cpp`, bank/string entry points | Interior guest cursors and numeric BASIC addresses | End-to-end bank/string/file vertical slice on identity and translated memory |
| `hal/files.h`, `host/hal_files.cpp` | File/lock tokens versus buffers and native objects | Tagged resource handles and directional span adapters |
| `runtime/marks.h`, `runtime/runtime.cpp`, `runtime/monitor.cpp` | Mark/context tokens and typed native continuations integrated | Preserve task/lifetime cleanup while migrating remaining guest registers/records |
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
| ~~Linux `-no-pie` in Makefile~~ | Removed: Linux builds biased by default and links as a PIE (140/140 host tests, 3 text runs under ASLR) |
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

## Current validation

- GCC: all 15 guest-contract tests and five native-runtime tests pass. This
  includes real static-image/dispatch relocation, high-backed code fields,
  compiler rejection checks, packed layouts and m68000 machine-code comparisons.
- Clang: the same contract/runtime suites pass; the container skips the Amiga
  machine-code comparison because its m68k compiler is not installed. That gate
  passes separately on this machine with the actual Amiga toolchain.
- Linux: 85 text tests pass, zero fail, 54 are skipped; four editor fixtures and
  the menus2 procedure/saved-register fixture pass.
- Windows and Amiga: builds pass. Amiga code-field and mapped-view operations
  add no instructions relative to their native load/store baselines. Saved-context
  capture and a3 access also match the native instruction baselines; live a5
  capture/load/store operations also add no instructions. The a4 load/store and
  relocation-byte append operations match the native instruction baselines.
- macOS arm64: cross-links with the startup guard retained; native interpreter
  execution remains unqualified.
- ASan/UBSan: the high-backed metadata/16-bit access/code-field view fixture passes,
  including permission checks and prevention of partial writes. The snapshot/mark
  registry fixture also passes with task ownership, nonlocal cleanup, saved guest
  pointer values and rejection of high native cursors. Live-a5 register transfers,
  packing, typed a4 relocation writes and low-backed record overlays pass the
  sanitizer fixture as well.

These checks cover the migrated boundaries. They do not qualify legacy native
data pointers, guest record overlays, platform mirrors or a complete high-address
interpreter. The remaining-work and workaround-removal checklists above are still
release gates; no usable native arm64 AMOS DMG is implied by cross-link success.
