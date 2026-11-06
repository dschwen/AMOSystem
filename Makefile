# Cross build of AMOS Professional from a Unix host.
#
#   make          build/AMOSPro, the program (C++, no assembler)
#   make dist     build/dist/AMOS: the AMOS tree (AMOS/, with the programs of
#                 examples/) with the freshly built AMOSPro and the editor's
#                 configuration with the blue palette
#   make test     run the AMOS test programs in FS-UAE (tests/run.py)
#   make host     build/host/amos: AMOS as a program of this computer
#   make install-host [PREFIX=~/.local]
#                 install it: PREFIX/bin/amos and the AMOS tree in PREFIX/share/amos
#   make uninstall-host [PREFIX=~/.local]
#   make install-hd
#                 replace AMOSPro on the emulator's hard disk (HD=amiga/harddisk),
#                 backing up the original to HD_BACKUP (amiga/backup)
#   make clean
#
# Needs m68k-amigaos-gcc/g++ from the Amiga cross toolchain
# (https://github.com/AmigaPorts/m68k-amigaos-gcc, in AMIGA_PREFIX) and python3:
# see README.md.

AMIGA_PREFIX ?= /opt/amiga
export PATH := $(AMIGA_PREFIX)/bin:$(PATH)

CC      := m68k-amigaos-gcc
CXX     := m68k-amigaos-g++
PYTHON  := python3
BUILD   := build
VERSION ?= 0.1.0
OBJ     := $(BUILD)/obj
DIST    := $(BUILD)/dist/AMOS
HOST    := $(BUILD)/host

# C++ for the 68000, no exceptions or RTTI, no C library startup: AMOS
# starts itself (src/amiga/start.cpp).
CPUFLAGS := -m68000
CXXFLAGS := $(CPUFLAGS) -Os -std=c++20 -fomit-frame-pointer -fno-exceptions -fno-rtti \
            -ffreestanding -fno-threadsafe-statics -Wall -Wextra -Werror -Isrc -MMD -MP
# libgcc for the arithmetic helpers, with libnix13 (Kickstart 1.3 compatible)
# supplying the ones this toolchain's libgcc leaves out (__divsi3...)
LDFLAGS  := $(CPUFLAGS) -nostdlib -nostartfiles
LDLIBS   := -L$(AMIGA_PREFIX)/m68k-amigaos/libnix/lib -lgcc -lnix13 -lgcc

# The C++ of AMOSPro, one directory for each part
#   src/lib      the routines of AMOSPro.Lib       src/w        AMOS.library
#   src/main     the start and the loaders         src/editor   the editor
#   src/verif    the verifier                      src/monitor  the monitor
#   src/interp   the interpreter                   src/hal      the system
#   src/ext      the extensions                    src/core     the layouts and the bridge
#   src/runtime  the glue's runtime                src/tables   the tables, data and entries
#   src/amiga    the machine code (start, entries the system calls)
AMIGA_DIRS   := src/lib src/main src/verif src/interp src/ext src/hal src/w src/editor src/monitor src/core \
                src/runtime src/tables
AMIGA_CXX    := $(sort $(wildcard $(addsuffix /*.cpp,$(AMIGA_DIRS))))
AMOSPRO_LINK := $(OBJ)/src/amiga/start.o $(OBJ)/src/amiga/native.o $(patsubst %.cpp,$(OBJ)/%.o,$(AMIGA_CXX))

.PHONY: linux-release package-linux dist-host all dist test test-reference install-hd host-check host host-sdl host-release game ndk install-host uninstall-host clean
.SECONDARY:
.DELETE_ON_ERROR:

all: $(BUILD)/AMOSPro

$(OBJ)/src/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# (their machine code in the order written)
$(OBJ)/src/amiga/%.o: src/amiga/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -fno-toplevel-reorder -c -o $@ $<

# (the tables are converted data: no warnings as errors)
$(OBJ)/src/tables/%.o: src/tables/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -Wno-error -c -o $@ $<

-include $(AMIGA_CXX:%.cpp=$(OBJ)/%.d)

# link/amospro.ld begins the code with the program's entry. This ld writes no
# memory flags: tools/chiphunks.py marks the chip hunks from the map.
$(BUILD)/AMOSPro: $(AMOSPRO_LINK) link/amospro.ld tools/chiphunks.py
	$(CC) $(LDFLAGS) -T link/amospro.ld -Wl,-Map,$@.map -o $@ $(AMOSPRO_LINK) $(LDLIBS)
	$(PYTHON) tools/chiphunks.py $@ $@.map

# ---- distribution --------------------------------------------------------------

dist: all
	rm -rf $(DIST) && mkdir -p $(dir $(DIST))
	cp -a AMOS $(DIST)
	cp -a examples/. $(DIST)/
	cp $(BUILD)/AMOSPro $(DIST)/AMOSPro
	$(PYTHON) tools/blue_palette.py AMOS/APSystem/AMOSPro_Editor_Config $(DIST)/APSystem/AMOSPro_Editor_Config

test: dist
	$(PYTHON) tests/run.py

test-reference:
	$(PYTHON) tests/run.py --reference

# The emulator's hard disk (amiga/, see README.md) with an installed AMOS Pro.
# Only the program is replaced; configs (which hold user settings) are left
# alone.
HD        ?= amiga/harddisk
HD_BACKUP ?= amiga/backup

install-hd: all
	$(PYTHON) tools/install_hd.py --build $(BUILD) --hd "$(HD)" --backup "$(HD_BACKUP)"

# ---- AMOS for another host -----------------------------------------------------
# 64 bits, AMOS's memory below 4GB: the portable C++, src/host (the HAL's
# backends) and src/tables.

HOST_SRC    := $(filter-out %_amiga.cpp src/amiga/%,$(wildcard src/*/*.cpp)) $(HOST)/topaz.cpp $(HOST)/keymap.cpp
HOST_OBJ    := $(patsubst %.cpp,$(HOST)/obj/%.o,$(HOST_SRC))
HOST_OPT    ?= -O0 -g
HOST_CXX    ?= g++
HOST_EXTRA_OBJ ?=
HOST_EXE    ?= amos
HOST_CPPFLAGS ?=
HOST_TARGET_OS ?= $(shell uname -s)
HOST_TARGET_ARCH ?= x86_64
HOST_ARCHFLAGS ?= -m64
# AMOS stores program and arena addresses in 32 bits. Darwin otherwise
# reserves the first 4GB and places the executable above them.
ifeq ($(HOST_TARGET_OS),Darwin)
ifeq ($(HOST_TARGET_ARCH),arm64)
HOST_LDFLAGS ?= -Wl,-pie
else
HOST_LDFLAGS ?= -Wl,-no_pie,-pagezero_size,0x1000
endif
else
HOST_LDFLAGS ?= -no-pie
endif
# SDL2: dlopen'ed when AMOS starts (default), or HOST_SDL=link: linked to
# the system's libSDL2-2.0.so.0 (no SDL headers needed)
HOST_SDL    ?= dlopen
HOST_SDLDEF := $(if $(filter link,$(HOST_SDL)),-DAMOS_SDL_LINKED)
HOST_SDLLIB := $(if $(filter link,$(HOST_SDL)),-l:libSDL2-2.0.so.0)
# The NDK's headers: the Amiga toolchain's if it is there, else NDK3.2's from
# Aminet (tools/fetch_ndk.py: build/ndk, no licence to bundle them: see it)
NDK_TOOLCHAIN := $(AMIGA_PREFIX)/m68k-amigaos/ndk-include
NDK_INCLUDE   ?= $(if $(wildcard $(NDK_TOOLCHAIN)/exec/types.h),$(NDK_TOOLCHAIN),$(BUILD)/ndk/Include_H)
HOST_FLAGS  := $(HOST_ARCHFLAGS) $(HOST_OPT) $(HOST_SDLDEF) -D__use_amiga_stdc_c99 -std=c++20 -fno-exceptions -fno-rtti -fno-threadsafe-statics -fpack-struct=2 \
               $(HOST_CPPFLAGS) -Isrc/host/include -Isrc -idirafter $(NDK_INCLUDE) -MMD -MP

# topaz and the usa keymap, from the user's Kickstart (as the tests'
# emulator has them); without one, AROS's topaz 8 (data/fonts/topaz8) and a
# plain keymap
KICKSTART ?= amiga/kickstarts/amiga-os-310-a1200.rom
ROMKEY ?= amiga/harddisk/Devs/Kickstarts/rom.key
$(HOST)/topaz.cpp: tools/extract_topaz.py $(wildcard data/fonts/topaz8/*.bin)
	@mkdir -p $(dir $@)
	$(PYTHON) tools/extract_topaz.py --out $@ --rom "$(KICKSTART)" --key "$(ROMKEY)" --fallback data/fonts/topaz8
$(HOST)/keymap.cpp: tools/extract_keymap.py
	@mkdir -p $(dir $@)
	$(PYTHON) tools/extract_keymap.py --out $@ --rom "$(KICKSTART)" --key "$(ROMKEY)"

# (the downloaded NDK headers, when there are no others)
$(BUILD)/ndk/Include_H/exec/types.h:
	$(PYTHON) tools/fetch_ndk.py --out $(BUILD)/ndk --cache $(BUILD)/download

ndk: $(BUILD)/ndk/Include_H/exec/types.h

$(HOST)/obj/%.o: %.cpp $(if $(filter $(BUILD)/ndk/Include_H,$(NDK_INCLUDE)),$(BUILD)/ndk/Include_H/exec/types.h)
	@mkdir -p $(dir $@)
	$(HOST_CXX) $(HOST_FLAGS) -c -o $@ $<

-include $(HOST_OBJ:%.o=%.d)

# Windows API types must retain their native layout across DLL boundaries.
# std::filesystem uses the native C++ library ABI, never guest packing.
$(HOST)/obj/src/host/installed_state.o: src/host/installed_state.cpp src/host/installed_state.h
	@mkdir -p $(dir $@)
	$(HOST_CXX) $(filter-out -fpack-struct=2,$(HOST_FLAGS)) -c -o $@ $<

$(HOST)/obj/src/host/windows_memory.o: src/host/windows_memory.cpp src/host/windows_memory.h
	@mkdir -p $(dir $@)
	$(HOST_CXX) $(filter-out -fpack-struct=2,$(HOST_FLAGS)) -c -o $@ $<

$(HOST)/obj/src/host/windows_platform.o: src/host/windows_platform.cpp src/host/windows_platform.h
	@mkdir -p $(dir $@)
	$(HOST_CXX) $(filter-out -fpack-struct=2,$(HOST_FLAGS)) -c -o $@ $<

# Translated memory owns native VM APIs; their ABI must not use guest packing.
$(HOST)/obj/src/host/guest_memory.o: src/host/guest_memory.cpp src/memory/guest_memory.h src/core/guest_types.h
	@mkdir -p $(dir $@)
	$(HOST_CXX) $(filter-out -fpack-struct=2,$(HOST_FLAGS)) -c -o $@ $<

.PHONY: test-guest-addressing
test-guest-addressing:
	CXX='$(HOST_CXX)' $(PYTHON) -m unittest discover -s tests -p 'test_guest*.py' -v
	CXX='$(HOST_CXX)' $(PYTHON) -m unittest discover -s tests -p 'test_runtime*.py' -v

host: $(HOST_OBJ) $(HOST_EXTRA_OBJ)
	$(HOST_CXX) $(HOST_ARCHFLAGS) $(HOST_LDFLAGS) -o $(HOST)/$(HOST_EXE) $(HOST_OBJ) $(HOST_EXTRA_OBJ) $(HOST_SDLLIB) > $(HOST)/link.txt 2>&1 || \
		{ cat $(HOST)/link.txt; exit 1; }
	grep -o "undefined reference to .*" $(HOST)/link.txt | sort -u \
		| tee $(HOST)/missing.txt | wc -l | sed 's/$$/ symbols missing (build\/host\/missing.txt)/'

# Linked to SDL2 instead of loading it, in build/host-sdl
.PHONY: host-windows windows-probe windows-dist windows-release package-msi msi-probe msi-tools windows-version
WINDOWS_CXX ?= x86_64-w64-mingw32-g++
WINDOWS_WINDRES ?= x86_64-w64-mingw32-windres
WINDOWS_BUILD ?= $(BUILD)/windows
WINDOWS_LDFLAGS ?= -Wl,--image-base,0x400000,--disable-dynamicbase,--disable-high-entropy-va -static-libgcc -static-libstdc++
SDL2_WINDOWS ?= $(HOME)/Downloads/SDL2-2.32.8/x86_64-w64-mingw32
host-windows:
	$(MAKE) host HOST=$(WINDOWS_BUILD) HOST_CXX=$(WINDOWS_CXX) HOST_EXE=amos.exe \
		HOST_EXTRA_OBJ='$(WINDOWS_BUILD)/version.o' \
		HOST_SDL=link HOST_CPPFLAGS='-I$(SDL2_WINDOWS)/include' \
		HOST_LDFLAGS='$(WINDOWS_LDFLAGS)' \
		HOST_SDLLIB='-L$(SDL2_WINDOWS)/lib -lmingw32 -lSDL2main -lSDL2 -lole32 -luuid -mwindows'

$(HOST)/version.rc: windows-version tools/windows_resources.py data/icons/amos.ico
	$(PYTHON) tools/windows_resources.py --version '$(VERSION)' --out $@

$(HOST)/version.o: $(HOST)/version.rc data/icons/amos.ico
	$(WINDOWS_WINDRES) -i $< -o $@

windows-release:
	@mkdir -p $(BUILD)/windows-release
	$(PYTHON) tools/extract_topaz.py --out $(BUILD)/windows-release/topaz.cpp --fallback data/fonts/topaz8
	$(PYTHON) tools/extract_keymap.py --out $(BUILD)/windows-release/keymap.cpp
	$(MAKE) host-windows WINDOWS_BUILD=$(BUILD)/windows-release HOST_OPT='-O2' KICKSTART= ROMKEY=

msi-tools:
	$(PYTHON) tools/bootstrap_msitools.py

msi-probe:
	$(PYTHON) tools/package_msi.py --feature-probe --version '$(VERSION)'

package-msi: windows-release
	$(PYTHON) tools/package_msi.py --version '$(VERSION)' --build $(BUILD)/windows-release --sdk '$(SDL2_WINDOWS)'

windows-probe: $(BUILD)/windows/windows-probe.exe

WINDOWS_PROBE_SRC := tests/windows/platform_probe.cpp tests/windows/native_probe.cpp tests/windows/backend_probe.cpp \
                     src/host/windows_memory.cpp src/host/windows_platform.cpp
$(BUILD)/windows/windows-probe.exe: $(WINDOWS_PROBE_SRC) src/host/windows_memory.h src/host/windows_platform.h src/host/windows_crt.h
	@mkdir -p $(dir $@)
	$(WINDOWS_CXX) -std=c++20 -O2 -g -Wall -Wextra -Werror -Isrc $(WINDOWS_LDFLAGS) \
		-static-libgcc -static-libstdc++ -o $@ $(WINDOWS_PROBE_SRC) -lshell32 -lole32 -luuid

windows-dist: host-windows windows-probe
	$(PYTHON) tools/package_windows.py --build $(BUILD)/windows --sdk '$(SDL2_WINDOWS)' --out $(BUILD)/windows-portable

host-sdl:
	$(MAKE) host HOST=$(BUILD)/host-sdl HOST_SDL=link

# The same, optimised, in build/host-release (what is installed)
host-release:
	$(MAKE) host HOST=$(BUILD)/host-release HOST_OPT='-O2'

# Install: the program, and the AMOS tree it runs in (it finds it from where it is: ../share/amos/AMOS),
# so that it does not matter where it is started. The tree is written to (the configurations), so
# the default is a user's own directory.
PREFIX ?= $(HOME)/.local

# Icon assets are checked in; Pillow is only needed when regenerating them.
.PHONY: icons macos-icons
icons:
	$(PYTHON) tools/generate_icons.py

# Run on the completed bundle before codesigning and creating its DMG.
macos-icons:
	@test -n "$(APP_BUNDLE)" || { echo 'set APP_BUNDLE to an existing .app'; exit 1; }
	$(PYTHON) tools/install_icons.py macos --app "$(APP_BUNDLE)"

install-host: host-release dist
	install -d $(DESTDIR)$(PREFIX)/bin $(DESTDIR)$(PREFIX)/share/amos
	install -m 755 $(BUILD)/host-release/amos $(DESTDIR)$(PREFIX)/bin/amos
	ln -sf amos $(DESTDIR)$(PREFIX)/bin/amos-player
	rm -rf $(DESTDIR)$(PREFIX)/share/amos/AMOS
	cp -a $(DIST) $(DESTDIR)$(PREFIX)/share/amos/AMOS
	$(PYTHON) tools/install_icons.py linux --prefix "$(DESTDIR)$(PREFIX)" $(if $(DESTDIR),,--refresh)
	@echo "installed $(DESTDIR)$(PREFIX)/bin/amos and $(DESTDIR)$(PREFIX)/share/amos/AMOS"

# The Linux release: ROM-free (AROS's topaz and a plain keymap, whatever Kickstart is around),
# optimised, as a portable tarball and a .deb in build/packages (docs/RELEASES.md). Needs git
# (the tree is staged from HEAD), and dpkg-deb and fakeroot for the .deb.
linux-release:
	@mkdir -p $(BUILD)/linux-release
	$(PYTHON) tools/extract_topaz.py --out $(BUILD)/linux-release/topaz.cpp --fallback data/fonts/topaz8
	$(PYTHON) tools/extract_keymap.py --out $(BUILD)/linux-release/keymap.cpp
	$(MAKE) host HOST=$(BUILD)/linux-release HOST_OPT='-O2' KICKSTART= ROMKEY=

package-linux: linux-release
	$(PYTHON) tools/package_linux.py --version '$(VERSION)' --build $(BUILD)/linux-release

dist-host: package-linux

# (the devices directories, the user's disks, are left: $${XDG_DATA_HOME:-~/.local/share}/amos/devices)
uninstall-host:
	$(PYTHON) tools/install_icons.py linux --prefix "$(DESTDIR)$(PREFIX)" --uninstall $(if $(DESTDIR),,--refresh)
	rm -f $(DESTDIR)$(PREFIX)/bin/amos $(DESTDIR)$(PREFIX)/bin/amos-player
	rm -rf $(DESTDIR)$(PREFIX)/share/amos/AMOS
	-rmdir $(DESTDIR)$(PREFIX)/share/amos
	@echo "uninstalled; the devices (disks) in $${XDG_DATA_HOME:-~/.local/share}/amos/devices are kept"

# Which C++ files compile for another host (tools/host_check.sh)
host-check:
	sh tools/host_check.sh

clean:
	rm -rf $(BUILD)

# A game for this computer: make game GAME=prog.AMOS [DATA=directory]
# [NAME=name] makes build/game/NAME, which runs on its own wherever it is
# put: NAME (the launcher), bin/amos-player (AMOS run only: no editor, no
# direct mode, it stops when the program does), share/amos/AMOS (the
# system files, without the examples and tools), share/amos/game (the
# program, and the files of DATA it loads: its START: and current folder)
NAME ?= $(basename $(notdir $(GAME)))
GAMEDIR := $(BUILD)/game/$(NAME)
game: host-release dist
	@test -n "$(GAME)" -a -f "$(GAME)" || { echo "make game GAME=prog.AMOS [DATA=dir] [NAME=name]"; exit 1; }
	rm -rf $(GAMEDIR)
	install -d $(GAMEDIR)/bin $(GAMEDIR)/share/amos/AMOS $(GAMEDIR)/share/amos/game
	install -m 755 $(BUILD)/host-release/amos $(GAMEDIR)/bin/amos-player
	cp -a $(DIST)/APSystem $(GAMEDIR)/share/amos/AMOS/
	rm -f $(GAMEDIR)/share/amos/AMOS/APSystem/*.info $(GAMEDIR)/share/amos/AMOS/APSystem/APCmp \
		$(GAMEDIR)/share/amos/AMOS/APSystem/*Compiler* $(GAMEDIR)/share/amos/AMOS/APSystem/Header_*
	cp -a $(DIST)/AMOSPro_Interpreter_Config $(GAMEDIR)/share/amos/AMOS/
	cp "$(GAME)" $(GAMEDIR)/share/amos/game/$(NAME).AMOS
	$(if $(DATA),cp -a "$(DATA)"/. $(GAMEDIR)/share/amos/game/)
	printf '#!/bin/sh\n# $(NAME): an AMOS program, run by AMOS'"'"'s player\nhere=$$(dirname "$$(readlink -f "$$0")")\ncd "$$here/share/amos/game" && exec "$$here/bin/amos-player" "$$here/share/amos/game/$(NAME).AMOS" "$$@"\n' > $(GAMEDIR)/$(NAME)
	chmod 755 $(GAMEDIR)/$(NAME)
	@echo "made $(GAMEDIR): run $(GAMEDIR)/$(NAME)"

# macOS distribution: build on macOS with Command Line Tools and SDL2.framework.
MACOS_CXX ?= /usr/bin/clang++ -arch x86_64
MACOS_BUILD ?= $(BUILD)/macos-release
MACOS_SDL_FRAMEWORK ?= /Library/Frameworks/SDL2.framework
MACOS_SIGN_IDENTITY ?= -
.PHONY: macos-release package-dmg dmg
macos-release:
	@test "$$(uname -s)" = Darwin || { echo 'DMG builds require macOS (run make package-dmg on the Mac)'; exit 1; }
	@mkdir -p $(MACOS_BUILD)
	$(PYTHON) tools/extract_topaz.py --out $(MACOS_BUILD)/topaz.cpp --fallback data/fonts/topaz8
	$(PYTHON) tools/extract_keymap.py --out $(MACOS_BUILD)/keymap.cpp
	MACOSX_DEPLOYMENT_TARGET=11.0 $(MAKE) host HOST=$(MACOS_BUILD) HOST_CXX='$(MACOS_CXX)' HOST_OPT=-O2 KICKSTART= ROMKEY=

package-dmg: macos-release
	$(PYTHON) tools/package_macos.py --version '$(VERSION)' --build $(MACOS_BUILD) --framework '$(MACOS_SDL_FRAMEWORK)' --sign-identity '$(MACOS_SIGN_IDENTITY)'

dmg: package-dmg

# Docker executes native Linux build tools and emits macOS Mach-O binaries.
MACOS_ARCH ?= arm64
MACOS_CROSS_IMAGE ?= amos-macos-cross:14.5
.PHONY: macos-cross-toolchain macos-cross-probes host-macos-cross
macos-cross-toolchain:
	$(PYTHON) tools/macos_cross.py toolchain --image '$(MACOS_CROSS_IMAGE)'
macos-cross-probes:
	$(PYTHON) tools/macos_cross.py probes --arch '$(MACOS_ARCH)' --image '$(MACOS_CROSS_IMAGE)'
host-macos-cross:
	$(PYTHON) tools/macos_cross.py host --arch '$(MACOS_ARCH)' --image '$(MACOS_CROSS_IMAGE)'
