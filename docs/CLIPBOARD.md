# Host clipboard integration

System text clipboard support is implemented for the SDL2 Linux, macOS and
Windows hosts. The native Amiga build retains its existing private block
clipboard and has no SDL dependency.

## Usage

Use **Ctrl+C/X/V** on Linux/Windows and **Command+C/X/V** on macOS. In the source
editor, select with the existing block marker (Ctrl+B in the default setup),
then move the cursor to the other end. Copy exports plain BASIC text; cut removes
it only after the OS clipboard write succeeds. Paste inserts at the cursor or
replaces the selection. Successful system copy/cut also updates the private
block for legacy block-paste commands. The host editor's **Clipboard** menu
provides System Copy/Cut/Paste without adding legacy function-table entries.
It uses a free top-level menu slot rather than replacing the configured Help menu.

In direct mode, copy/cut uses the selected range, or the entire command when
nothing is selected. Shift+Left/Right/Home/End and Shift+mouse placement select;
Home/End move to the command boundaries. Paste inserts at the cursor or replaces
the selection. **Paste never executes a command:** press Return explicitly.
A single final newline is accepted and removed; multiline commands are rejected.
Running BASIC and BASIC INPUT retain their Ctrl+C interrupt behavior. The Devices
overlay and other dialog line editors do not edit the underlying source.

## Limits and implementation

OS clipboard text is UTF-8. The initial source conversion uses printable
ISO-8859-1 bytes, consistent with the repository's source listing tools;
accented Latin-1 text round-trips. The fallback AROS font draws byte 164 as a
euro sign, whereas this byte-preserving source mapping exports it as U+00A4.
Custom fonts do not change source byte semantics. Characters outside printable
Latin-1 (including emoji/euro), invalid UTF-8 and control sequences are rejected
rather than replaced. CRLF/CR normalize to LF, and each tab becomes one space,
matching ASCII merge. Normal tokenizer/detokenizer formatting still applies.

Clipboard processing is limited to 1 MiB of UTF-8, 250 source bytes per resulting
line, the tokenized-line/program-buffer limits, and direct mode's initialized
160-byte capacity. Source cut/paste requires an enabled undo buffer. Each source
edit is one Undo/Redo action, backed by before/after token-buffer snapshots that
also retain cursor, selection and marks. Candidate validation and guest-memory
allocation precede the edit; clipboard-write failure does not delete a cut.
These snapshots use additional low-address memory proportional to program size.

Unfold closed procedures before copying/editing a selection that contains one;
the initial adapter refuses closed selections to preserve their hidden token
representation. Empty/non-text/unavailable system clipboards give feedback and
do not paste stale private data. Without an SDL window, system clipboard access
is unavailable. Clipboard persistence after AMOS exits depends on the platform
and desktop clipboard manager.

The bridge lives in `src/host/sdl.cpp`; both dynamic and linked SDL modes bind
the same API. Operations run synchronously at editor key/menu dispatch or in the
explicitly enabled direct-mode line loop, on the same cooperative host thread
as SDL. No background queue or document mutation in input/VBL callbacks is used.
Physical key release resets a chord latch so guest auto-repeat does not repeat
cuts/pastes. No guest structures or legacy function IDs were expanded. Host
strings are never passed as guest pointers: tokenizer scratch and undo records
use the low-memory allocator.

## Validation

Run after `make host`:

```sh
python3 -m unittest discover -s tests -p test_clipboard.py
make host-sdl
AMOS_CLIPBOARD_TEST_BINARY="$PWD/build/host-sdl/amos" \
  python3 -m unittest discover -s tests -p test_clipboard.py
python3 tests/host_run.py --text-only
make host-windows
make
```

The clipboard test requires Xvfb, SDL2, X11/XTest and Pillow. It creates an
isolated X11 clipboard owned by a separate SDL window, so it never accesses the
user's desktop clipboard. Tests cover actual mouse selection of System Copy/Cut/Paste as well as source
copy/paste/cut/undo/redo,
blank lines, invalid/unrepresentable and overlong paste, direct-mode Unicode,
selection replacement, capacity and multiline rejection, held keys, BASIC INPUT
interruption, and explicit Return before executing a marker command. Pure C++
conversion checks cover malformed UTF-8 and the processing limit. These tests
provide X11 coverage; Windows, macOS and Wayland interactive validation remains
required. The virtual Mac was unreachable (`ssh mac`: connection refused) during
implementation; no Mac build/runtime result is claimed.

## Original design and remaining platform qualification

The design below records the intended behavior and broader qualification matrix.
The implementation choices and current limitations above take precedence where
they differ (synchronous dispatch, closed-procedure refusal, and byte mapping).

## Approach and existing code

Use SDL2's text clipboard API for all three host operating systems. Extend
`src/host/sdl.cpp`'s `Sdl` function table, linked declarations/initialization,
and dynamic symbol loading with GetClipboardText, SetClipboardText and
HasClipboardText; free returned memory with the existing SDL_free binding.
Keep OS clipboard access in the SDL event/video thread. No xclip/wl-copy,
pbpaste, PowerShell helpers or separate native clipboard implementations are
needed for the first version. Test Linux X11 and Wayland separately.

The relevant SDL contracts are UTF-8 input/output, explicit ownership of returned
memory, and a failure result for writes:

- [SDL_GetClipboardText](https://wiki.libsdl.org/SDL2/SDL_GetClipboardText)
- [SDL_SetClipboardText](https://wiki.libsdl.org/SDL2/SDL_SetClipboardText)
- [SDL_HasClipboardText](https://wiki.libsdl.org/SDL2/SDL_HasClipboardText)

The editor's private `Ed_Block` in `src/editor/block.cpp` mixes text fragments
with tokenized middle lines, including closed-procedure metadata. Never publish
that representation as clipboard text. `src/editor/blocksave.cpp` already
contains a `BlockText` iterator for ASCII export; factor out its useful
selection/detokenization logic, but do not inherit its omission of empty edge
lines or its unconditional final newline without checking selection boundaries.
`src/editor/disk.cpp`'s ASCII merge and `src/editor/edit.cpp`/`lines.cpp` provide
insertion/tokenization paths to reuse rather than importing BASIC as keystrokes.

Direct mode is separate: `src/editor/direct.cpp::escape_loop` initializes a
`LineEdit` with a 160-character capacity and calls `L_LEd_Loop`.
`src/lib/lineedit.cpp` owns cursor movement, rendering and buffer edits. It does
not currently provide a general selection model. Clipboard support must target
this buffer, not the editor's source selection or rendered screen pixels.

## User-visible behavior

| Context | Copy | Cut | Paste |
|---|---|---|---|
| Source editor | Selected source as plain text | Copy selection, then delete after successful system write | Insert system text at cursor, replacing an active selection |
| Direct mode | Selection, or the entire editable command when no selection exists | Selection, or the entire command, after successful system write | Insert at cursor or replace selection; never submit the command |

Use Ctrl+C/X/V on Linux and Windows, Command+C/X/V on macOS. Add explicit
System Copy/Cut/Paste editor menu commands so the feature is discoverable and
legacy block commands remain available. Keep existing private block clipboard
commands intact. Mirror successful system copy/cut into the private block where
appropriate, but explicit system paste always reads current OS contents; an
empty/non-text/unavailable clipboard is a no-op with feedback, not a fallback
to stale private text. Copy with no source selection does not clear the OS
clipboard. Never clear the clipboard on focus loss or exit.

Intercept shortcuts only when the source editor or direct-mode command buffer
actually owns input. An executing BASIC program must still receive Ctrl+C.
The Devices overlay, dialogs, search fields and other line editors must not
accidentally route commands to the underlying source editor. Do not extend
clipboard support to arbitrary BASIC INPUT fields in this initial scope.

Add direct-mode selection using Shift+Left/Right/Home/End and Shift+mouse cursor
placement; keep the anchor/selection in external C++ editor state rather than
expanding the guest `LineEdit` layout. Clear or clamp selection on ordinary
navigation, edits, history recall, command submission and mode changes. Render
the selection using the line editor's existing cursor/scroll coordinate model.
Whole-command copy makes initial direct-mode support useful even without a
selection; selection support is still required for the finished feature.

Direct-mode multiline paste must not execute commands. Normalize line endings,
remove at most one final newline from a single-line clipboard, then reject any
remaining newline with a visible explanation. Do not silently join statements
or queue Return events. Users paste multiline BASIC into the source editor.

## Text conversion and bounded edits

Treat OS text as UTF-8 and AMOS source as its existing single-byte character
set. Audit the actual source/font mapping before choosing the conversion table;
do not assume every byte is Latin-1 just because some tooling uses it. Document
and test the mapping, including accented characters. Preserve original AMOS
bytes on an internal block operation; UTF-8 belongs at the host boundary.

Normalize CRLF and lone CR to LF. Preserve empty lines, indentation, quotes,
comments, tabs according to a documented editor-compatible tab policy, and
whether a selected fragment ends in a newline. Reject malformed UTF-8, embedded
NUL/control sequences other than accepted whitespace, and unrepresentable
characters with a useful message before changing the document. Avoid silent
replacement, truncation or Unicode normalization that changes literals.

Preflight total size, each resulting line's actual storage/tokenizer limit,
selection bounds, available memory and direct-mode remaining capacity. Start
with an explicit 1 MiB host text limit, additionally constrained by AMOS memory
and format limits; report exceeding any limit. The host can own native strings,
but anything passed to guest routines must be copied into low-address allocated
memory. Never put a native host pointer into an AMOS register or guest field.
SDL allocates a complete read result before application size validation, so the
application limit bounds processing/insertion rather than SDL's allocation.

Build a complete candidate edit before mutating the live document. Commit paste
or cut as one undoable action, with correct modified flag, line count, cursor,
selection, tokenization and redraw. Existing block cut/paste call `undo_reset()`;
simply routing through those routines will not satisfy this requirement.
Implement the necessary compound undo support, or snapshot-and-restore through
the established undo machinery. Allocation/tokenization failure must leave the
original text and cursor intact. A failed system write must not delete a cut
selection. Detokenizing closed procedures must not unexpectedly unfold or
change them; follow the existing editor's protected-procedure rules.

## Implementation sequence

1. Introduce a small clipboard HAL interface with explicit available/empty/error
   results and read/write operations. Add host SDL implementation and Amiga
   unavailable stub; no new SDL dependency in Amiga objects. Provide a fake
   clipboard for deterministic tests. Verify both linked and dynamically loaded
   SDL hosts, and graceful unavailable behavior without a display.
2. Add a bounded command queue between SDL shortcut capture and editor-owned
   safe points in `editor/loop.cpp` and direct mode. Clipboard operations run
   where SDL is serviced; document edits run only with valid editor context.
   Carry mode/generation IDs and discard requests after focus/mode/document
   changes. Define completion/error handling for cut before deleting anything.
   Do not mutate a document inside a VBL/input callback. If a synchronous call
   is safe under the current task/fiber model, establish that with code tracing
   first instead of introducing a blocking cross-thread wait.
3. Factor selection-to-text conversion and text-to-edit validation/insertion;
   add system source Copy/Cut/Paste commands, transactional undo and menu items.
   Account for current uncommitted line buffers, partial lines and collapsed
   procedures. Keep legacy editor command IDs and native Amiga behavior stable.
4. Add direct-mode buffer copy/cut/paste, selection rendering and navigation,
   capacity checks and the multiline rejection policy. Reuse low-level buffer
   editing helpers while keeping clipboard shortcuts limited to direct mode.
5. Wire platform shortcuts in SDL event handling. Consume each recognized
   chord once, including repeats and matching releases, while maintaining guest
   modifier state; avoid stuck Ctrl/Command and duplicate guest operations.
   Preserve overlay priority and normal input for running programs. Validate
   Command handling on the existing macOS SDL framework build.
6. Finish platform validation, user documentation and release notes. No installer
   association or packaging changes should be needed beyond shipping the updated
   executable; retain SDL2 as the only host clipboard backend initially.

## Validation and acceptance

Unit tests with a fake clipboard cover source selection extraction, token/text
round trips, newline boundaries, blank lines, high bytes, Unicode rejection,
malformed data, size limits, and failure before commit. Cover undo/redo for
selection replacement and multiline paste, failed cut writes, allocation
failure, stale queued requests and changed clipboard ownership. Test direct
mode cursor insertion, selection replacement, history reset, full capacity and
multiline rejection. A pasted command that writes a marker must not create it
until the user explicitly presses Return.

Run real bidirectional copy/paste against an external text editor on Linux X11,
Linux Wayland, macOS and Windows. Check shortcuts and menu commands, clipboard
changes while AMOS is unfocused, a second AMOS process, closed procedures,
partial/multiple source lines, direct-mode selected/whole-command copy, and
non-US keyboard layouts. Verify Ctrl+C still interrupts running BASIC, and
that dialogs/overlay do not edit the background document. Test both Linux SDL
link modes, macOS framework loading and the Windows linked SDL build. Record
platform-specific clipboard persistence after exit rather than promising it
without testing the SDL/backend ownership behavior.

Regression gates: normal Linux host suite with existing image comparisons,
focused clipboard editor/direct-mode integration tests, Windows cross-build
and backend probe, macOS build/runtime checks, and Amiga cross-compilation/link.
Headless tests must remain deterministic without reading the real clipboard.
Do not claim Wayland coverage from Xvfb/X11 tests or host runtime coverage from
successful cross-compilation alone.
