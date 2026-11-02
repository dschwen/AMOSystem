# Why AMOS-visible addresses must fit in 32 bits

The host executable is already a 64-bit program. Its C++ pointers are 64 bits,
and libraries may use memory anywhere in the process address space. The
restriction is narrower: **any native address stored in an AMOS 32-bit address
field must be representable without losing bits**. With the current direct
pointer model, those objects must occupy virtual addresses below
`0x1_0000_0000` (4 GiB). Zero remains the null address.

This is an address-space requirement, not a requirement for physical RAM
below 4 GiB, and not a promise that the whole 4 GiB is available to AMOS. The
host currently reserves a 512 MiB main arena, plus separate process resources.
Some original chip-memory behavior has still narrower address assumptions.

## Where the requirement comes from

AMOS was written around the Amiga's 32-bit programming model: 32-bit address
registers, four-byte address fields, and structures accessed at fixed byte
offsets. The original 68000 has a narrower external address bus, but that is
a separate issue from the 32-bit pointer representation preserved here.

The C++ conversion retains these representations so the original data layouts,
resources, and program behavior continue to work. It does not translate every
memory access through a complete emulated address space.

The central conversions are in [core/amos.h](../src/core/amos.h):

```cpp
address(p)       // native pointer -> uint32_t, truncating to 32 bits
ptr<T>(value)    // uint32_t -> native pointer
```

There is a special mapping for the emulated low vector page, but ordinary
addresses are otherwise interpreted directly. For example, truncating the
native address `0x0000000110000000` produces `0x10000000`. Converting that value
back does not recover the original address. It may reference an unmapped page,
or a different valid allocation, causing silent corruption instead of a crash.

Objects affected include:

- AMOS allocations: programs, banks, screens, strings, and system structures.
- Generated tables and native entry addresses stored in four-byte relocation
  slots. Allocating only the main arena low does not fix high executable data.
- Interpreter-visible stack objects. [runtime/runtime.cpp](../src/runtime/runtime.cpp)
  stores addresses of `Mark` objects in fields such as `BasSp`; they contain
  the native `setjmp` state used for error recovery and interpreter restarts.
- Host objects whose addresses are returned through emulated registers or
  stored in AMOS structures, such as library bases and device objects.

A host-only allocation that never crosses this boundary need not be low.
For example, the Windows allocator's bookkeeping can live above 4 GiB.

## Why virtual memory cannot always supply a suitable address

Virtual memory can map arbitrary physical pages into an available low virtual
range. Physical RAM need not be contiguous or physically below 4 GiB. However,
**virtual memory does not give an application ownership of every address in its
process**. The loader and OS have already reserved parts of that space.

A request can fail for several independent reasons:

1. **Existing mappings and reservations.** The executable, DLLs, heaps, stacks,
   guard pages, and platform-reserved regions can occupy low addresses.
   The Windows probe found the desired 1 MiB chip address occupied even while
   the much larger arena allocation succeeded elsewhere.
2. **Contiguous virtual-space requirements.** A 512 MiB arena needs one suitably
   aligned free interval of that size. Having 512 MiB free in scattered holes
   is insufficient. Page/allocation alignment further reduces usable space.
3. **Loader policy and architecture constraints.** The initial Intel macOS
   executable reserved the first 4 GiB as `__PAGEZERO` and loaded its text at
   4 GiB. The Intel linker workaround changes that layout. It is not a general
   guarantee for every architecture, OS version, SDK, or signing configuration;
   native Apple Silicon requires separate investigation.
4. **ASLR and allocation order.** Placement changes between runs or machines.
   A successful search/probe demonstrates a particular environment, not an
   unconditional OS contract. Other allocations can also race a free-space
   query, so the actual reservation must still handle failure safely.
5. **APIs that choose addresses internally.** Windows `CreateFiberEx` accepts
   stack sizes and an entry point, but no caller-supplied stack address. Our
   arena allocator cannot dictate where that API places a fiber stack.
6. **Resource limits.** Even a suitable virtual range does not guarantee that
   reservation/commit operations can satisfy memory limits or commitment
   charges. Mapping and physical-page availability are related but distinct.

A normal allocation hint may return another address. A constrained allocator
may instead fail if the permitted range has no suitable space. Neither behavior
makes a high result safe to truncate. Windows `VirtualAlloc2` can express
address requirements, but it cannot create a free interval where none exists.
See Microsoft's [VirtualAlloc](https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc)
and [VirtualAlloc2](https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc2)
contracts for placement, alignment, reservation, and commitment behavior.

Forcing an address does not make occupied space safe to use. Linux `MAP_FIXED`
can replace overlapping mappings; `MAP_FIXED_NOREPLACE` instead fails on an
occupied region. Clobbering the executable, stack, or a library is not a memory
allocation strategy. [Linux mmap documentation](https://man7.org/linux/man-pages/man2/mmap.2.html)

Likewise, moving an existing allocation or mapping an alias does not generally
repair pointers already stored in C++ objects, return addresses, relocations,
or OS bookkeeping. The runtime must control the representation and translation
of those references, not just arrange a second mapping of some pages.

## What the current backends do

Linux x86-64 uses `MAP_32BIT` for the arena (which specifically targets the
first 2 GiB on that platform), non-PIE executable linking, and a separately
allocated low interpreter stack. The low chip mapping has an arena fallback.

Intel macOS uses Darwin-specific linker options to reduce PAGEZERO and disable
PIE, and switches the main thread onto an interpreter stack. Keeping the main
thread is needed for native window handling. These are Intel macOS measures,
not a verified native arm64 solution.

Windows x86-64 links the image at a low preferred base with ASLR disabled for
this experimental build. [windows_memory.cpp](../src/host/windows_memory.cpp)
searches free virtual regions and reserves/commits an exact low arena without
replacing existing mappings. The backend validates the complete image range
before interpreter initialization and the active stack bounds on entry to
each interpreter/task fiber. An incompatible placement exits with a diagnostic.
It does not attempt to relocate an OS-created fiber stack.

The user's Windows probe passed image, arena, fiber-stack, recovery, and
word-call tests. The public [CreateFiberEx interface](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-createfiberex)
still makes no promise about the stack's numerical address. The checks remain
necessary. Current protection is backend-specific: `address()` itself is not
a universal checked conversion, so new AMOS-visible pointer sources need review.

## Why changing the sources to use 64-bit pointers is substantial work

Changing the compiler's pointer width is already done. Replacing `uint32_t`
with `uintptr_t` everywhere would change AMOS's model as well as native storage:

- [core/layout.h](../src/core/layout.h) and [core/ioreq.h](../src/core/ioreq.h)
  preserve fixed field widths and offsets, with static assertions. Widening
  address fields moves later fields and breaks routines accessing byte offsets.
- Emulated registers, parameter stacks, relocation slots, and big-endian
  four-byte reads/writes carry addresses without a C++ pointer type attached.
  A typedef change cannot identify which four-byte values should widen.
- BASIC integers and exposed addresses overlap semantically. Programs use
  `Start`, `Varptr`, `Peek`/`Leek`, `Poke`/`Doke`/`Loke`, and address arithmetic.
  Widening every integer would alter language behavior; widening selected
  pointers requires tracking what the integer represents.
- Serialized programs, banks, configurations, and generated tables have
  established formats. Host-private references must be separated from these
  formats rather than accidentally writing eight-byte native pointers into
  four-byte records. Amiga ABI encodings such as BPTR need their own treatment.
- Native function references, stack marks, guest data, and emulated handles
  are different categories. An address that can be dereferenced and incremented
  cannot always be replaced by an opaque ID. Conversely, a native function or
  jump buffer need not be made into ordinary guest-readable data.

This is not an impossibility claim. The durable alternative is to separate
**32-bit guest addresses** from **native pointers**: map guest addresses to
host memory through an explicit translator, preserve guest arithmetic and
formats, and use separate registries/adapters for native functions and runtime
control state. Guest-visible stack data must also be accounted for. A single
change to `ptr()`/`address()` is insufficient while raw casts, interior pointers,
fixed-offset accesses, and native stack addresses still bypass that boundary.

Such a refactor could remove the low-address requirement while keeping AMOS's
32-bit language and file compatibility. It requires a systematic pointer-flow
and layout audit plus regression tests, with the Amiga implementation retaining
its native ABI. The [Windows](WINDOWS.md) and [Apple Silicon](APPLE-SILICON.md)
plans distinguish this longer-term design from the current placement-based
host ports.
