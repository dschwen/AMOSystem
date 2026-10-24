# Cross build of AMOS Professional from a Unix host.
#
#   make          assemble everything into build/
#   make dist     build/dist/AMOS: the shipped AMOS tree with the freshly
#                 built binaries copied over the prebuilt ones
#   make verify   compare the binaries that are still pure assembler with the
#                 prebuilt ones in AMOS/
#   make test     run the AMOS test programs in FS-UAE (tests/run.py)
#   make install-hd
#                 replace the AMOS Pro binaries on the FS-UAE Workbench drive
#                 (HD=...), backing up the originals to HD_BACKUP
#   make clean
#
# Needs vasmm68k_mot, m68k-amigaos-gcc/g++ and m68k-amigaos-objdump from the
# Amiga cross toolchain (https://github.com/AmigaPorts/m68k-amigaos-gcc) and
# python3.

AMIGA_PREFIX ?= /opt/amiga
export PATH := $(AMIGA_PREFIX)/bin:$(PATH)

VASM    := vasmm68k_mot
CC      := m68k-amigaos-gcc
CXX     := m68k-amigaos-g++
PYTHON  := python3
BUILD   := build
GEN     := $(BUILD)/gen
OBJ     := $(BUILD)/obj
CI      := $(BUILD)/ci
DIST    := $(BUILD)/dist/AMOS

# GenAm/GenIm2 compatible settings. EzFlag selects the Easy AMOS variant of
# AMOS.library and was set on the original command line.
INCLUDES := -I$(CI) -I. -I$(GEN) -Iincludes -Iextensions
ASFLAGS  := -quiet -devpac -nosym -DEzFlag=0 $(INCLUDES)
EXEFLAGS := $(ASFLAGS) -Fhunkexe -kick1hunks
OBJFLAGS := $(ASFLAGS) -Fhunk -kick1hunks

# C++ for the converted parts. 68000 code, no exceptions or RTTI, no C
# library startup: the code runs inside AMOS, called through asm glue.
CPUFLAGS := -m68000
CXXFLAGS := $(CPUFLAGS) -Os -std=c++20 -fomit-frame-pointer -fno-exceptions -fno-rtti \
            -ffreestanding -fno-threadsafe-statics -Wall -Wextra -Werror -Isrc -I$(GEN) -MMD -MP
# libgcc for the arithmetic helpers, with libnix13 (Kickstart 1.3 compatible)
# supplying the ones this toolchain's libgcc leaves out (__divsi3...)
LDFLAGS  := $(CPUFLAGS) -nostdlib -nostartfiles
LDLIBS   := -L$(AMIGA_PREFIX)/m68k-amigaos/libnix/lib -lgcc -lnix13 -lgcc

# C++ and assembler sources of the converted parts, one directory per
# program they are linked into; src/core is linked into all of them.
#   src/lib      routines moved out of AMOSPro.Lib   -> AMOSPro
#   src/main     the main program                    -> AMOSPro
#   src/verif    the verifier (+Verif.s)             -> AMOSPro
#   src/interp   the interpreter core (+ILib.s)      -> AMOSPro
#   src/w        AMOS.library (linked into AMOSPro)
#   src/editor   AMOSPro_Editor
#   src/monitor  AMOSPro_Monitor
#   src/core     the C++/asm bridge and data layout (an archive)
src_objs = $(strip $(patsubst %.s,$(OBJ)/%.o,$(sort $(wildcard $(addsuffix /*.s,$(1))))) \
           $(patsubst %.cpp,$(OBJ)/%.o,$(sort $(wildcard $(addsuffix /*.cpp,$(1))))))
OBJ_AMOSPRO := $(call src_objs,src/lib src/main src/verif src/interp src/ext)
OBJ_W       := $(call src_objs,src/w)
OBJ_EDITOR  := $(call src_objs,src/editor)
OBJ_MONITOR := $(call src_objs,src/monitor)
OBJ_COMMON  := $(call src_objs,src/core)
COMMON_LIB  := $(OBJ)/libcommon.a
ALL_CXX     := $(shell find src -name '*.cpp')

# link_program(output, linker script, objects): link, then mark chip hunks
define link_program
	$(CC) $(LDFLAGS) -T $(2) -Wl,-Map,$(1).map -o $(1) $(3) $(COMMON_LIB) $(LDLIBS)
	$(PYTHON) tools/chiphunks.py $(1) $(1).map
endef

# The generated headers carry a fixed date so that builds are reproducible
STAMP := 01-01-2026 00:00:00

LIB_GEN := $(GEN)/+LEqu.s $(GEN)/+Internal_Jumps.s $(GEN)/+Lib_Size.s \
           $(GEN)/+ILib_Size.s $(GEN)/+ILib_Functions.s $(GEN)/lib_routines.h
TOKTAB  := $(GEN)/+Toktab_Verif.Bin
EXTENSIONS := Music Compact Request IO_Ports
EXT_GEN := $(foreach e,$(EXTENSIONS),$(GEN)/+$(e)_Size.s $(GEN)/+$(e)_Labels.s $(GEN)/+$(e)_Jumps.s)

COMMON_SRC := +AMOS_Includes.s +Equ.s +CEqu.s +WEqu.s +Debug.s +Version.s

BINARIES := $(BUILD)/AMOSPro $(BUILD)/AMOSPro.Lib \
            $(BUILD)/AMOSPro_Music.Lib $(BUILD)/AMOSPro_Compact.Lib \
            $(BUILD)/AMOSPro_Request.Lib $(BUILD)/AMOSPro_IOPorts.Lib \
            $(BUILD)/AMOSPro_Editor_Config $(BUILD)/AMOSPro_Interpreter_Config

# Built file -> prebuilt reference, relative to AMOS/
REFERENCE_AMOSPro                    := AMOSPro
REFERENCE_AMOSPro_Interpreter_Config := AMOSPro_Interpreter_Config
REFERENCE_DEFAULT                     = APSystem/$(1)
reference = AMOS/$(or $(REFERENCE_$(1)),$(call REFERENCE_DEFAULT,$(1)))

.PHONY: all dist verify test test-reference install-hd clean
.SECONDARY:
.DELETE_ON_ERROR:

all: $(BINARIES)

# ---- generated sources -------------------------------------------------------

$(LIB_GEN) &: +ILib.s +Lib.s tools/make_labels.py | $(GEN)
	$(PYTHON) tools/make_labels.py --src . --out $(GEN) --stamp '$(STAMP)'

# The C++ view of the assembler data structures
CXX_GEN := $(GEN)/core/layout.h $(GEN)/lib_routines.h

$(GEN)/core/layout.h: +Equ.s +CEqu.s +WEqu.s +MEqu.s extensions/+Music_Data.s tools/gen_layout.py $(LIB_GEN) $(CI)/.stamp
	@mkdir -p $(dir $@)
	$(PYTHON) tools/gen_layout.py --out $@ --vasm $(VASM) -DEzFlag=0 $(INCLUDES)

$(TOKTAB): +Lib.s tools/make_toktable.py | $(GEN)
	$(PYTHON) tools/make_toktable.py $< $@

$(GEN)/+%_Size.s $(GEN)/+%_Labels.s $(GEN)/+%_Jumps.s &: extensions/+%.s tools/library_digest.py tools/make_labels.py | $(GEN)
	$(PYTHON) tools/library_digest.py $< --out $(GEN) --stamp '$(STAMP)'

# Symlinks for includes whose case differs from the file on disk
$(CI)/.stamp: tools/case_links.py | $(GEN)
	rm -rf $(CI) && mkdir -p $(CI)
	$(PYTHON) tools/case_links.py --out $(CI) -I $(GEN) -I . -I includes -I extensions \
		+B.s +W.s +Lib.s +Edit.s +Monitor.s +Editor_Config.s +Interpreter_Config.s \
		$(foreach e,$(EXTENSIONS),extensions/+$(e).s)
	touch $@

$(GEN):
	mkdir -p $@

PREREQ := $(LIB_GEN) $(CI)/.stamp $(COMMON_SRC)

# ---- binaries -----------------------------------------------------------------

# Programs: the assembler source becomes one object, linked with the C++ of
# its src/ directory. The linker scripts keep the hunk order the assembler
# code depends on.
$(OBJ)/src/%.o: src/%.s $(wildcard src/*.i src/*/*.i)
	@mkdir -p $(dir $@)
	$(VASM) $(OBJFLAGS) -Isrc -o $@ $<

$(OBJ)/src/%.o: src/%.cpp $(CXX_GEN)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(COMMON_LIB): $(OBJ_COMMON)
	@mkdir -p $(dir $@)
	rm -f $@
	$(if $^,m68k-amigaos-ar rcs $@ $^,m68k-amigaos-ar rcs $@)

-include $(ALL_CXX:%.cpp=$(OBJ)/%.d)

$(OBJ)/B.o: +B.s +Verif.s $(PREREQ) $(EXT_GEN)
	@mkdir -p $(dir $@)
	$(VASM) $(OBJFLAGS) -o $@ $<

# AMOS.library, the editor and the monitor are linked in too (main/boot.cpp
# starts AMOS.library from _w_debut, main/modules.cpp finds the entry
# tables of the others)
AMOSPRO_LINK := $(OBJ)/B.o $(OBJ_AMOSPRO) $(OBJ)/gen/lib_direct.o $(OBJ)/gen/lib_direct_entries.o $(OBJ)/gen/w_direct.o $(OBJ)/W.o $(OBJ_W) $(OBJ)/Edit.o $(OBJ_EDITOR) \
                $(OBJ)/Monitor.o $(OBJ_MONITOR)
$(BUILD)/AMOSPro: $(AMOSPRO_LINK) $(COMMON_LIB) link/amospro.ld tools/chiphunks.py
	$(call link_program,$@,link/amospro.ld,$(AMOSPRO_LINK))

# AMOS.library was built with GenIm2, which matches symbols without case
$(OBJ)/W.o: +W.s $(PREREQ) $(wildcard bin/*)
	@mkdir -p $(dir $@)
	$(VASM) $(OBJFLAGS) -nocase -o $@ $<


$(BUILD)/AMOSPro.Lib: +Lib.s +ILib.s $(PREREQ) $(TOKTAB) bin/+Dialog_Funcs.bin
	$(VASM) $(EXEFLAGS) -o $@ $<

# The same library, linked into AMOSPro (main/libraries.cpp loads it from
# there)
$(OBJ)/src/main/library.o: src/main/library.s +Lib.s +ILib.s $(PREREQ) $(TOKTAB) bin/+Dialog_Funcs.bin
	@mkdir -p $(dir $@)
	$(VASM) $(OBJFLAGS) -o $@ $<

$(OBJ)/Edit.o: +Edit.s $(PREREQ)
	@mkdir -p $(dir $@)
	$(VASM) $(OBJFLAGS) -o $@ $<


$(OBJ)/Monitor.o: +Monitor.s $(PREREQ)
	@mkdir -p $(dir $@)
	$(VASM) $(OBJFLAGS) -o $@ $<


$(BUILD)/AMOSPro_IOPorts.Lib: extensions/+IO_Ports.s $(GEN)/+IO_Ports_Size.s $(PREREQ)
	$(VASM) $(EXEFLAGS) -o $@ $<

$(BUILD)/AMOSPro_%.Lib: extensions/+%.s $(GEN)/+%_Size.s $(PREREQ)
	$(VASM) $(EXEFLAGS) -o $@ $<

# The extensions, linked into AMOSPro too (main/libraries.cpp)
$(OBJ)/src/ext/music_image.o: src/ext/music_image.s extensions/+Music.s $(GEN)/+Music_Size.s $(PREREQ) $(EXT_GEN)
	@mkdir -p $(dir $@)
	$(VASM) $(OBJFLAGS) -o $@ $<
$(OBJ)/src/ext/compact_image.o: src/ext/compact_image.s extensions/+Compact.s $(GEN)/+Compact_Size.s $(PREREQ) $(EXT_GEN)
	@mkdir -p $(dir $@)
	$(VASM) $(OBJFLAGS) -o $@ $<
$(OBJ)/src/ext/request_image.o: src/ext/request_image.s extensions/+Request.s $(GEN)/+Request_Size.s $(PREREQ) $(EXT_GEN)
	@mkdir -p $(dir $@)
	$(VASM) $(OBJFLAGS) -o $@ $<
$(OBJ)/src/ext/ioports_image.o: src/ext/ioports_image.s extensions/+IO_Ports.s $(GEN)/+IO_Ports_Size.s $(PREREQ) $(EXT_GEN)
	@mkdir -p $(dir $@)
	$(VASM) $(OBJFLAGS) -o $@ $<

# The C++ functions of the library routines, for direct calls (core/amos.h)
$(GEN)/lib_direct.cpp $(GEN)/lib_direct_entries.s: tools/lib_direct.py $(wildcard src/*/*_glue.s) $(GEN)/lib_routines.h
	$(PYTHON) tools/lib_direct.py --out $(GEN)/lib_direct.cpp --entries $(GEN)/lib_direct_entries.s \
		--routines $(GEN)/lib_routines.h $(wildcard src/*/*_glue.s)
$(OBJ)/gen/lib_direct.o: $(GEN)/lib_direct.cpp $(CXX_GEN)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(GEN)/w_direct.cpp: tools/w_direct.py +W.s $(wildcard src/w/*_glue.s)
	$(PYTHON) tools/w_direct.py --out $@ +W.s $(wildcard src/w/*_glue.s)
$(OBJ)/gen/w_direct.o: $(GEN)/w_direct.cpp $(CXX_GEN)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(OBJ)/gen/lib_direct_entries.o: $(GEN)/lib_direct_entries.s
	@mkdir -p $(dir $@)
	$(VASM) $(OBJFLAGS) -o $@ $<

# Configs are raw data: the contents of the single hunk, padded to a longword
# (what c/genim2 followed by c/Get_Chunk produced)
$(BUILD)/AMOSPro_%_Config: +%_Config.s $(PREREQ) $(wildcard bin/*)
	$(VASM) $(ASFLAGS) -Fbin -o $@ $<
	$(PYTHON) -c "import sys; p=sys.argv[1]; d=open(p,'rb').read(); open(p,'wb').write(d+bytes(-len(d)%4))" $@

# ---- distribution and verification ------------------------------------------

dist: all
	rm -rf $(DIST) && mkdir -p $(dir $(DIST))
	cp -a AMOS $(DIST)
	$(foreach b,$(BINARIES),cp $(b) $(DIST)/$(subst AMOS/,,$(call reference,$(notdir $(b))));)

# Programs with C++ in them no longer match the prebuilt binaries
CONVERTED := $(BUILD)/AMOSPro $(BUILD)/AMOSPro.Lib

VERIFY := $(filter-out $(CONVERTED) %_Config,$(BINARIES))

verify: all
	@status=0; \
	for b in $(VERIFY); do \
		n=$$(basename $$b); \
		case $$n in AMOSPro) r=AMOS/AMOSPro;; *) r=AMOS/APSystem/$$n;; esac; \
		$(PYTHON) tools/bincmp.py $$r $$b || status=1; \
	done; \
	cmp $(BUILD)/AMOSPro_Editor_Config AMOS/APSystem/AMOSPro_Editor_Config \
		&& echo "$(BUILD)/AMOSPro_Editor_Config: identical" || status=1; \
	cmp $(BUILD)/AMOSPro_Interpreter_Config AMOS/AMOSPro_Interpreter_Config \
		&& echo "$(BUILD)/AMOSPro_Interpreter_Config: identical" || status=1; \
	exit $$status

test: dist
	$(PYTHON) tests/run.py

test-reference:
	$(PYTHON) tests/run.py --reference

# The FS-UAE directory drive with an installed AMOS Pro. Only programs and
# libraries are replaced; configs (which hold user settings) are left alone.
HD        ?= ../Minimal Workbench v3.1
HD_BACKUP ?= ../AMOS_Pro-original-binaries

install-hd: all
	$(PYTHON) tools/install_hd.py --build $(BUILD) --hd "$(HD)" --backup "$(HD_BACKUP)"

clean:
	rm -rf $(BUILD)
