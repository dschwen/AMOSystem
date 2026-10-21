# Cross build of AMOS Professional from a Unix host.
#
#   make          assemble everything into build/
#   make dist     build/dist/AMOS: the shipped AMOS tree with the freshly
#                 built binaries copied over the prebuilt ones
#   make verify   compare the built binaries with the prebuilt ones in AMOS/
#   make test     run the AMOS test programs in FS-UAE (tests/run.py)
#   make install-hd
#                 replace the AMOS Pro binaries on the FS-UAE Workbench drive
#                 (HD=...), backing up the originals to HD_BACKUP
#   make clean
#
# Needs vasmm68k_mot and m68k-amigaos-objdump from the Amiga cross toolchain
# (https://github.com/AmigaPorts/m68k-amigaos-gcc) and python3.

AMIGA_PREFIX ?= /opt/amiga
export PATH := $(AMIGA_PREFIX)/bin:$(PATH)

VASM    := vasmm68k_mot
PYTHON  := python3
BUILD   := build
GEN     := $(BUILD)/gen
CI      := $(BUILD)/ci
DIST    := $(BUILD)/dist/AMOS

# GenAm/GenIm2 compatible settings. EzFlag selects the Easy AMOS variant of
# AMOS.library and was set on the original command line.
INCLUDES := -I$(CI) -I. -I$(GEN) -Iincludes -Iextensions
ASFLAGS  := -quiet -devpac -nosym -DEzFlag=0 $(INCLUDES)
EXEFLAGS := $(ASFLAGS) -Fhunkexe -kick1hunks

# The generated headers carry a fixed date so that builds are reproducible
STAMP := 01-01-2026 00:00:00

LIB_GEN := $(GEN)/+LEqu.s $(GEN)/+Internal_Jumps.s $(GEN)/+Lib_Size.s \
           $(GEN)/+ILib_Size.s $(GEN)/+ILib_Functions.s
TOKTAB  := $(GEN)/+Toktab_Verif.Bin
EXTENSIONS := Music Compact Request IO_Ports
EXT_GEN := $(foreach e,$(EXTENSIONS),$(GEN)/+$(e)_Size.s $(GEN)/+$(e)_Labels.s)

COMMON_SRC := +AMOS_Includes.s +Equ.s +CEqu.s +WEqu.s +Debug.s +Version.s

BINARIES := $(BUILD)/AMOSPro $(BUILD)/AMOS.library $(BUILD)/AMOSPro.Lib \
            $(BUILD)/AMOSPro_Editor $(BUILD)/AMOSPro_Monitor \
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

$(TOKTAB): +Lib.s tools/make_toktable.py | $(GEN)
	$(PYTHON) tools/make_toktable.py $< $@

$(GEN)/+%_Size.s $(GEN)/+%_Labels.s &: extensions/+%.s tools/library_digest.py tools/make_labels.py | $(GEN)
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

$(BUILD)/AMOSPro: +B.s +Verif.s $(PREREQ)
	$(VASM) $(EXEFLAGS) -o $@ $<

# AMOS.library was built with GenIm2, which matches symbols without case
$(BUILD)/AMOS.library: +W.s $(PREREQ) $(wildcard bin/*)
	$(VASM) $(EXEFLAGS) -nocase -o $@ $<

$(BUILD)/AMOSPro.Lib: +Lib.s +ILib.s $(PREREQ) $(TOKTAB) bin/+Dialog_Funcs.bin
	$(VASM) $(EXEFLAGS) -o $@ $<

$(BUILD)/AMOSPro_Editor: +Edit.s $(PREREQ)
	$(VASM) $(EXEFLAGS) -o $@ $<

$(BUILD)/AMOSPro_Monitor: +Monitor.s $(PREREQ)
	$(VASM) $(EXEFLAGS) -o $@ $<

$(BUILD)/AMOSPro_IOPorts.Lib: extensions/+IO_Ports.s $(GEN)/+IO_Ports_Size.s $(PREREQ)
	$(VASM) $(EXEFLAGS) -o $@ $<

$(BUILD)/AMOSPro_%.Lib: extensions/+%.s $(GEN)/+%_Size.s $(PREREQ)
	$(VASM) $(EXEFLAGS) -o $@ $<

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

verify: all
	@status=0; \
	for b in $(filter-out %_Config,$(BINARIES)); do \
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
