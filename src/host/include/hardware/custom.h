// The custom chips' registers on another host (in place of the NDK's
// hardware/custom.h): the same layout, each register an object whose
// reads and writes go to the emulation of the chips (host/hal_chips.cpp):
// a write to DMACON sets or clears bits, one to BLTSIZE starts the
// blitter, and so on. The pointer registers keep 32-bit addresses.
#ifndef HARDWARE_CUSTOM_H
#define HARDWARE_CUSTOM_H

#include <stddef.h>
#include <stdint.h>

#include <exec/types.h>

extern "C" {
// host/hal_chips.cpp: the register at offset (from $DFF000)
uint16_t host_chip_read(uint32_t offset);
void host_chip_write(uint32_t offset, uint16_t value);
uint32_t host_chip_offset(const volatile void *reg);
}

struct ChipReg {
    uint16_t raw;

    uint32_t offset() const volatile { return host_chip_offset(this); }
    operator uint16_t() const volatile { return host_chip_read(offset()); }
    void operator=(uint16_t v) volatile { host_chip_write(offset(), v); }
    void operator|=(uint16_t v) volatile { *this = uint16_t(*this | v); }
    void operator&=(uint16_t v) volatile { *this = uint16_t(*this & v); }
};

// Two registers: the high word first
struct ChipLong {
    ChipReg hi, lo;

    operator uint32_t() const volatile { return uint32_t(uint16_t(hi)) << 16 | uint16_t(lo); }
    void operator=(uint32_t v) volatile
    {
        hi = uint16_t(v >> 16);
        lo = uint16_t(v);
    }
};

// A pointer register: an address of AMOS's memory (below 4GB)
struct ChipPtr {
    ChipReg hi, lo;

    uint32_t value() const volatile { return uint32_t(uint16_t(hi)) << 16 | uint16_t(lo); }
    operator void *() const volatile { return reinterpret_cast<void *>(uintptr_t(value())); }
    template <typename T> void operator=(T *p) volatile { set(uint32_t(reinterpret_cast<uintptr_t>(p))); }
    void operator=(uint32_t v) volatile { set(v); }
    void set(uint32_t v) volatile
    {
        hi = uint16_t(v >> 16);
        lo = uint16_t(v);
    }
};

struct Custom {
    ChipReg bltddat, dmaconr, vposr, vhposr, dskdatr, joy0dat, joy1dat, clxdat, adkconr, pot0dat, pot1dat,
        potinp, serdatr, dskbytr, intenar, intreqr;
    ChipPtr dskpt;
    ChipReg dsklen, dskdat, refptr, vposw, vhposw, copcon, serdat, serper, potgo, joytest, strequ, strvbl,
        strhor, strlong;
    ChipReg bltcon0, bltcon1, bltafwm, bltalwm;
    ChipPtr bltcpt, bltbpt, bltapt, bltdpt;
    ChipReg bltsize;
    UBYTE pad2d;
    UBYTE bltcon0l;
    ChipReg bltsizv, bltsizh;
    ChipReg bltcmod, bltbmod, bltamod, bltdmod;
    ChipReg pad34[4];
    ChipReg bltcdat, bltbdat, bltadat;
    ChipReg pad3b[3];
    ChipReg deniseid, dsksync;
    ChipLong cop1lc, cop2lc;
    ChipReg copjmp1, copjmp2, copins, diwstrt, diwstop, ddfstrt, ddfstop, dmacon, clxcon, intena, intreq,
        adkcon;
    struct AudChannel {
        ChipPtr ac_ptr;
        ChipReg ac_len, ac_per, ac_vol, ac_dat;
        ChipReg ac_pad[2];
    } aud[4];
    ChipPtr bplpt[8];
    ChipReg bplcon0, bplcon1, bplcon2, bplcon3, bpl1mod, bpl2mod, bplcon4, clxcon2;
    ChipReg bpldat[8];
    ChipPtr sprpt[8];
    struct SpriteDef {
        ChipReg pos, ctl, dataa, datab;
    } spr[8];
    ChipReg color[32];
    ChipReg htotal, hsstop, hbstrt, hbstop, vtotal, vsstop, vbstrt, vbstop, sprhstrt, sprhstop, bplhstrt,
        bplhstop, hhposw, hhposr, beamcon0, hsstrt, vsstrt, hcenter, diwhigh;
    ChipReg padf3[11];
    ChipReg fmode;
};

static_assert(offsetof(Custom, bltcon0) == 0x40);
static_assert(offsetof(Custom, bltsize) == 0x58);
static_assert(offsetof(Custom, cop1lc) == 0x80);
static_assert(offsetof(Custom, dmacon) == 0x96);
static_assert(offsetof(Custom, aud) == 0xA0);
static_assert(offsetof(Custom, bplpt) == 0xE0);
static_assert(offsetof(Custom, sprpt) == 0x120);
static_assert(offsetof(Custom, color) == 0x180);
static_assert(offsetof(Custom, fmode) == 0x1FC);

#endif // HARDWARE_CUSTOM_H
