# Cross build of AMOS Professional from a Unix host.
#
#   make          build/AMOSPro, the program (C++, no assembler)
#   make dist     build/dist/AMOS: the shipped AMOS tree with the freshly built
#                 AMOSPro and the editor's configuration with the blue palette
#   make test     run the AMOS test programs in FS-UAE (tests/run.py)
#   make host     build/host/amos: AMOS as a program of this computer
#   make install-host [PREFIX=~/.local]
#                 install it: PREFIX/bin/amos and the AMOS tree in PREFIX/share/amos
#   make uninstall-host [PREFIX=~/.local]
#   make install-hd
#                 replace AMOSPro on the FS-UAE Workbench drive (HD=...),
#                 backing up the original to HD_BACKUP
#   make clean
#
# Needs m68k-amigaos-gcc/g++ from the Amiga cross toolchain
# (https://github.com/AmigaPorts/m68k-amigaos-gcc) and python3.

AMIGA_PREFIX ?= /opt/amiga
export PATH := $(AMIGA_PREFIX)/bin:$(PATH)

CC      := m68k-amigaos-gcc
CXX     := m68k-amigaos-g++
PYTHON  := python3
BUILD   := build
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

.PHONY: all dist test test-reference install-hd host-check host host-release install-host uninstall-host clean
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
	cp $(BUILD)/AMOSPro $(DIST)/AMOSPro
	$(PYTHON) tools/blue_palette.py AMOS/APSystem/AMOSPro_Editor_Config $(DIST)/APSystem/AMOSPro_Editor_Config

test: dist
	$(PYTHON) tests/run.py

test-reference:
	$(PYTHON) tests/run.py --reference

# The FS-UAE directory drive with an installed AMOS Pro. Only the program is
# replaced; configs (which hold user settings) are left alone.
HD        ?= ../Minimal Workbench v3.1
HD_BACKUP ?= ../AMOS_Pro-original-binaries

install-hd: all
	$(PYTHON) tools/install_hd.py --build $(BUILD) --hd "$(HD)" --backup "$(HD_BACKUP)"

# ---- AMOS for another host -----------------------------------------------------
# 64 bits, AMOS's memory below 4GB: the portable C++, src/host (the HAL's
# backends) and src/tables.

HOST_SRC    := $(filter-out %_amiga.cpp src/amiga/%,$(wildcard src/*/*.cpp)) $(HOST)/topaz.cpp
HOST_OBJ    := $(patsubst %.cpp,$(HOST)/obj/%.o,$(HOST_SRC))
HOST_OPT    ?= -O0 -g
HOST_FLAGS  := -m64 $(HOST_OPT) -D__use_amiga_stdc_c99 -std=c++20 -fno-exceptions -fno-rtti -fno-threadsafe-statics -fpack-struct=2 \
               -Isrc/host/include -Isrc -idirafter $(AMIGA_PREFIX)/m68k-amigaos/ndk-include -MMD -MP

# topaz, from the user's Kickstart (as the tests' emulator has it)
KICKSTART ?= $(HOME)/Programs/CAMOS/FS-UAE/Kickstarts/amiga-os-310-a1200.rom
ROMKEY ?= $(HOME)/Programs/CAMOS/Minimal Workbench v3.1/Devs/Kickstarts/rom.key
$(HOST)/topaz.cpp: tools/extract_topaz.py
	@mkdir -p $(dir $@)
	$(PYTHON) tools/extract_topaz.py --out $@ --rom "$(KICKSTART)" --key "$(ROMKEY)"

$(HOST)/obj/%.o: %.cpp
	@mkdir -p $(dir $@)
	g++ $(HOST_FLAGS) -c -o $@ $<

-include $(HOST_OBJ:%.o=%.d)

host: $(HOST_OBJ)
	g++ -m64 -no-pie -o $(HOST)/amos $(HOST_OBJ) > $(HOST)/link.txt 2>&1 || \
		{ grep -v "undefined reference" $(HOST)/link.txt | grep -i "error\|multiple" ; true; }
	grep -o "undefined reference to .*" $(HOST)/link.txt | sort -u \
		| tee $(HOST)/missing.txt | wc -l | sed 's/$$/ symbols missing (build\/host\/missing.txt)/'

# The same, optimised, in build/host-release (what is installed)
host-release:
	$(MAKE) host HOST=$(BUILD)/host-release HOST_OPT='-O2'

# Install: the program, and the AMOS tree it runs in (it finds it from where it is: ../share/amos/AMOS),
# so that it does not matter where it is started. The tree is written to (the configurations), so
# the default is a user's own directory.
PREFIX ?= $(HOME)/.local

install-host: host-release dist
	install -d $(DESTDIR)$(PREFIX)/bin $(DESTDIR)$(PREFIX)/share/amos
	install -m 755 $(BUILD)/host-release/amos $(DESTDIR)$(PREFIX)/bin/amos
	rm -rf $(DESTDIR)$(PREFIX)/share/amos/AMOS
	cp -a $(DIST) $(DESTDIR)$(PREFIX)/share/amos/AMOS
	@echo "installed $(DESTDIR)$(PREFIX)/bin/amos and $(DESTDIR)$(PREFIX)/share/amos/AMOS"

# (the devices directories, the user's disks, are left: $${XDG_DATA_HOME:-~/.local/share}/amos/devices)
uninstall-host:
	rm -f $(DESTDIR)$(PREFIX)/bin/amos
	rm -rf $(DESTDIR)$(PREFIX)/share/amos/AMOS
	-rmdir $(DESTDIR)$(PREFIX)/share/amos
	@echo "uninstalled; the devices (disks) in $${XDG_DATA_HOME:-~/.local/share}/amos/devices are kept"

# Which C++ files compile for another host (tools/host_check.sh)
host-check:
	sh tools/host_check.sh

clean:
	rm -rf $(BUILD)
