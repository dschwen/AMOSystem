// The host's own picture over AMOS's (docs/DEVICES.md, 5.3): the Devices
// panel (F12) with a folder browser, the message of what happened to a disk,
// and the hint while AMOS waits for one ("Please insert volume X"). Drawn
// into the frame (host/display.cpp's 768x288 pixels) with the topaz 8 the
// host has, so it is the same on every system and in the player. The keys and
// the mouse are the panel's while it is open (host/sdl.cpp sends them); AMOS
// goes on behind it.

#include "host/devices.h"
#include "host/romfont.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "host/cxx.h"

#include "host/windows_crt.h"

#if defined(_WIN32)
#include "host/windows_platform.h"
#define realpath(path, ignored) amos::host::windows::canonical_path(path)
#define mkdir(path, mode) amos::host::windows::make_directory(path)
#endif

// Private host objects containing native C++ library members.
#pragma pack(push, 16)
namespace amos::host {

void drop(const char *spec);       // drops.cpp
void message(const char *text);


namespace {

constexpr int W = 768, H = 288, Cols = W / 8, Rows = H / 8;
// the panel: columns and rows of 8x8 cells
constexpr int PCol = 3, PRow = 2, PCols = Cols - 6, PRows = Rows - 5;
constexpr uint32_t Back = 0x0B1B4D, Edge = 0xFFFFFF, Fg = 0xFFFFFF, Dim = 0xA0B4D8, Select = 0xFFCC33, SelectFg = 0x000000,
                   Button = 0x3A5AA8, Good = 0x1E6B2E, Warn = 0x8A2B2B;

// SDL's scancodes
enum Key { KReturn = 40, KEscape = 41, KBackspace = 42, KEnterPad = 88, KUp = 82, KDown = 81, KPageUp = 75, KPageDown = 78,
           KHome = 74, KEnd = 77, KE = 8, KI = 12, KM = 16, KS = 22, KW = 26 };

enum class Mode { off, panel, browser };
Mode mode = Mode::off;
int selected;                       // the row of the panel
std::string toast, hint_volume;
double toast_until;

struct Browser {
    bool mount;                     // a hard disk (else a disk for unit)
    int unit;
    std::string dir;
    std::vector<std::string> folders;
    int cursor = 0, top = 0;
} browser;
std::string last_dir;

struct Click {
    double at = 0;
    int row = -1;
} last_click;

uint32_t *fb;

double clock_seconds()
{
#if defined(_WIN32)
    return double(amos::host::windows::monotonic_microseconds()) / 1e6;
#else
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return double(t.tv_sec) + double(t.tv_nsec) / 1e9;
#endif
}

// ---- drawing ---------------------------------------------------------------

const RomFont *font()
{
    for (int i = 0; i < rom_font_count; i++)
        if (rom_fonts[i].ysize == 8 && rom_fonts[i].data)
            return &rom_fonts[i];
    return nullptr;
}

void pixel(int x, int y, uint32_t colour)
{
    if (x >= 0 && x < W && y >= 0 && y < H)
        fb[y * W + x] = colour;
}

void fill(int col, int row, int cols, int rows, uint32_t colour)
{
    for (int y = row * 8; y < (row + rows) * 8 && y < H; y++)
        for (int x = col * 8; x < (col + cols) * 8 && x < W; x++)
            fb[y * W + x] = colour;
}

void box(int col, int row, int cols, int rows)
{
    for (int x = col * 8; x < (col + cols) * 8; x++) {
        pixel(x, row * 8 + 1, Edge);
        pixel(x, (row + rows) * 8 - 2, Edge);
    }
    for (int y = row * 8; y < (row + rows) * 8; y++) {
        pixel(col * 8 + 1, y, Edge);
        pixel((col + cols) * 8 - 2, y, Edge);
    }
}

// A text at a cell, at most maxcols characters; a background colour if bg is not 0xFFFFFFFF
constexpr uint32_t None = 0xFFFFFFFF;
void text(int col, int row, const std::string &s, uint32_t fg, uint32_t bg = None, int maxcols = Cols)
{
    const RomFont *f = font();
    for (int i = 0; i < int(s.size()) && i < maxcols; i++) {
        unsigned char c = static_cast<unsigned char>(s[size_t(i)]);
        int x0 = (col + i) * 8, y0 = row * 8;
        if (!f || c < f->lo || c > f->hi)
            c = '?';
        uint32_t loc = f ? f->locs[c - f->lo] : 0;
        int offset = int(loc >> 16), width = int(loc & 0xFFFF);
        for (int y = 0; y < 8; y++)
            for (int x = 0; x < 8; x++) {
                bool on = f && x < width && (f->data[y * f->modulo + ((offset + x) >> 3)] >> (7 - ((offset + x) & 7)) & 1);
                if (on)
                    pixel(x0 + x, y0 + y, fg);
                else if (bg != None)
                    pixel(x0 + x, y0 + y, bg);
            }
    }
}

// A path cut from the left to fit
std::string tail(const std::string &s, size_t n) { return s.size() <= n ? s : "..." + s.substr(s.size() - (n - 3)); }
std::string head(const std::string &s, size_t n) { return s.size() <= n ? s : s.substr(0, n); }

// ---- the units ---------------------------------------------------------------

// The units that are there, in order
std::vector<int> unit_rows()
{
    std::vector<int> rows;
    devices::UnitInfo info;
    for (int u = 0; u < devices::Units; u++)
        if (devices::unit_info(u, info) && info.exists)
            rows.push_back(u);
    return rows;
}

struct Rect {
    int x, y, w, h;
    char key;
};
std::vector<Rect> buttons;

void draw_panel()
{
    fill(PCol, PRow, PCols, PRows, Back);
    box(PCol, PRow, PCols, PRows);
    text(PCol + 2, PRow, " Devices ", Select, Back);
    text(PCol + 2, PRow + 2, "Unit   Disk                        Folder", Dim);
    std::vector<int> rows = unit_rows();
    if (selected >= int(rows.size()))
        selected = int(rows.size()) - 1;
    if (selected < 0)
        selected = 0;
    for (int i = 0; i < int(rows.size()); i++) {
        devices::UnitInfo info;
        devices::unit_info(rows[size_t(i)], info);
        bool on = i == selected;
        int y = PRow + 3 + i;
        if (on)
            fill(PCol + 1, y, PCols - 2, 1, Select);
        uint32_t fg = on ? SelectFg : Fg, dim = on ? SelectFg : Dim;
        uint32_t bg = on ? Select : None;
        text(PCol + 2, y, std::string(info.name) + ":", fg, bg);
        if (info.has_disk) {
            text(PCol + 9, y, head(info.label, 26), fg, bg);
            text(PCol + 36, y, tail(info.path, 38), dim, bg);
            if (info.read_only)
                text(PCol + 76, y, "protected", fg, bg);
        } else {
            text(PCol + 9, y, "(no disk)", dim, bg);
        }
    }
    // the buttons
    buttons.clear();
    static const struct {
        const char *label;
        char key;
    } actions[] = {{" Insert (I) ", 'i'}, {" Eject (E) ", 'e'}, {" Protect (W) ", 'w'},
                   {" Mount hard disk (M) ", 'm'}, {" Save (S) ", 's'}, {" Close (F12) ", 'x'}};
    int x = PCol + 2, y = PRow + PRows - 6;
    for (const auto &a : actions) {
        text(x, y, a.label, Fg, Button);
        buttons.push_back({x, y, int(strlen(a.label)), 1, a.key});
        x += int(strlen(a.label)) + 1;
    }
    text(PCol + 2, PRow + PRows - 4, "Drop a folder on the window to mount it as a hard disk.", Dim);
    std::string library = "Library: ";
    for (int i = 0; i < devices::library_count(); i++)
        library += (i ? ", " : "") + std::string(devices::library_path(i));
    if (!devices::library_count())
        library += "none (set it up in the setup file)";
    text(PCol + 2, PRow + PRows - 3, tail(library, size_t(PCols - 4)), Dim);
    text(PCol + 2, PRow + PRows - 2, tail(std::string("Setup file: ") + devices::config_path(), size_t(PCols - 4)), Dim);
}

// ---- the folder browser ----------------------------------------------------------

void list_folders()
{
    browser.folders.clear();
    if (DIR *d = opendir(browser.dir.c_str())) {
        while (struct dirent *e = readdir(d)) {
            if (e->d_name[0] == '.')
                continue;
            std::string path = browser.dir + "/" + e->d_name;
            struct stat st;
            if (stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
                browser.folders.push_back(e->d_name);
        }
        closedir(d);
    }
    std::sort(browser.folders.begin(), browser.folders.end(),
              [](const std::string &a, const std::string &b) { return strcasecmp(a.c_str(), b.c_str()) < 0; });
    browser.cursor = browser.top = 0;
}

void browse(const std::string &where)
{
    char *real = realpath(where.c_str(), nullptr);
    browser.dir = real ? real : "/";
    free(real);
    list_folders();
}

// The items: "use this folder", the parent (but at the top), the folders
int browser_items() { return 1 + (browser.dir != "/" ? 1 : 0) + int(browser.folders.size()); }

std::string browser_item(int i)
{
    if (i == 0)
        return "[ Use this folder ]";
    if (browser.dir != "/") {
        if (i == 1)
            return "..";
        return browser.folders[size_t(i - 2)] + "/";
    }
    return browser.folders[size_t(i - 1)] + "/";
}

void start_browser(bool mount, int unit)
{
    browser.mount = mount;
    browser.unit = unit;
    if (last_dir.empty()) {
        const char *start = getenv("AMOS_START_DIR"), *home = getenv("HOME");
        last_dir = devices::library_count() ? devices::library_path(0) : start && *start ? start : home ? home : "/";
    }
    browse(last_dir);
    mode = Mode::browser;
}

constexpr int ListRows = PRows - 7;

void draw_browser()
{
    fill(PCol, PRow, PCols, PRows, Back);
    box(PCol, PRow, PCols, PRows);
    std::string title = " A folder as a hard disk ";
    if (!browser.mount) {
        devices::UnitInfo info;
        devices::unit_info(browser.unit, info);
        title = std::string(" A folder as the disk of ") + info.name + ": ";
    }
    text(PCol + 2, PRow, title, Select, Back);
    text(PCol + 2, PRow + 2, tail(browser.dir, size_t(PCols - 4)), Fg);
    int items = browser_items();
    if (browser.cursor < browser.top)
        browser.top = browser.cursor;
    if (browser.cursor >= browser.top + ListRows)
        browser.top = browser.cursor - ListRows + 1;
    for (int r = 0; r < ListRows; r++) {
        int i = browser.top + r;
        if (i >= items)
            break;
        bool on = i == browser.cursor;
        int y = PRow + 4 + r;
        if (on)
            fill(PCol + 1, y, PCols - 2, 1, Select);
        text(PCol + 2, y, head(browser_item(i), size_t(PCols - 4)), on ? SelectFg : i == 0 ? Select : Fg, on ? Select : None);
    }
    text(PCol + 2, PRow + PRows - 2, "Up/Down, PageUp/PageDown: choose   Enter: open or use   Backspace: up   Esc: back", Dim);
}

void choose()
{
    last_dir = browser.dir;
    std::string spec = browser.dir;
    if (!browser.mount) {
        devices::UnitInfo info;
        devices::unit_info(browser.unit, info);
        spec = std::string(info.name) + "=" + browser.dir;
    }
    mode = Mode::panel;
    drop(spec.c_str());
}

void activate(int i)
{
    if (i == 0) {
        choose();
    } else if (browser.dir != "/" && i == 1) {
        browse(browser.dir.substr(0, browser.dir.rfind('/') == 0 ? 1 : browser.dir.rfind('/')));
    } else {
        std::string name = browser.folders[size_t(i - (browser.dir != "/" ? 2 : 1))];
        browse(browser.dir == "/" ? "/" + name : browser.dir + "/" + name);
    }
}

// ---- the panel's actions ------------------------------------------------------------

void action(char key)
{
    std::vector<int> rows = unit_rows();
    int unit = selected >= 0 && selected < int(rows.size()) ? rows[size_t(selected)] : -1;
    devices::UnitInfo info;
    switch (key) {
    case 'i':
        if (unit >= 0)
            start_browser(false, unit);
        break;
    case 'm':
        start_browser(true, -1);
        break;
    case 'e':
        if (unit >= 0 && devices::unit_info(unit, info) && info.has_disk)
            drop((std::string(info.name) + "=").c_str());
        break;
    case 'w':
        if (unit >= 0 && devices::unit_info(unit, info) && info.has_disk) {
            devices::set_read_only(unit, !info.read_only);
            message((std::string(info.name) + (info.read_only ? ": can be written" : ": write-protected")).c_str());
        }
        break;
    case 's': {
        const char *why = "";
        if (devices::save_config(why))
            message((std::string("Saved ") + devices::config_path()).c_str());
        else
            message((std::string("Cannot save ") + devices::config_path() + ": " + why).c_str());
        break;
    }
    case 'x':
        mode = Mode::off;
        break;
    }
}

} // namespace

// What happened to a disk: on stderr, and in the window for a few seconds
void message(const char *text)
{
    fprintf(stderr, "amos: %s\n", text);
    toast = text;
    toast_until = clock_seconds() + 5;
}

// The disk AMOS is waiting for (null: none)
void hint(const char *volume) { hint_volume = volume ? volume : ""; }

namespace overlay {

bool active() { return mode != Mode::off; }

bool visible() { return mode != Mode::off || !hint_volume.empty() || (!toast.empty() && clock_seconds() < toast_until); }

void toggle()
{
    mode = mode == Mode::off ? Mode::panel : Mode::off;
}

void key(int scancode, bool down)
{
    if (!down)
        return;
    if (mode == Mode::browser) {
        int items = browser_items();
        switch (scancode) {
        case KUp: browser.cursor = std::max(0, browser.cursor - 1); break;
        case KDown: browser.cursor = std::min(items - 1, browser.cursor + 1); break;
        case KPageUp: browser.cursor = std::max(0, browser.cursor - ListRows); break;
        case KPageDown: browser.cursor = std::min(items - 1, browser.cursor + ListRows); break;
        case KHome: browser.cursor = 0; break;
        case KEnd: browser.cursor = items - 1; break;
        case KReturn:
        case KEnterPad: activate(browser.cursor); break;
        case KBackspace:
            if (browser.dir != "/")
                activate(1);
            break;
        case KEscape: mode = Mode::panel; break;
        }
        return;
    }
    switch (scancode) {
    case KUp: selected = std::max(0, selected - 1); break;
    case KDown: selected++; break;
    case KReturn:
    case KEnterPad: action('i'); break;
    case KEscape: mode = Mode::off; break;
    case KI: action('i'); break;
    case KE: action('e'); break;
    case KW: action('w'); break;
    case KM: action('m'); break;
    case KS: action('s'); break;
    }
}

void mouse(int x, int y, int button, bool down)
{
    // (the window's pixels to the frame's cells)
    int col = x / 8, row = y / 8;
    if (button != 1 || !down)
        return;
    double now = clock_seconds();
    if (mode == Mode::panel) {
        for (const Rect &b : buttons)
            if (col >= b.x && col < b.x + b.w && row == b.y) {
                action(b.key);
                return;
            }
        int i = row - (PRow + 3);
        if (col > PCol && col < PCol + PCols - 1 && i >= 0 && i < int(unit_rows().size())) {
            bool twice = last_click.row == i && now - last_click.at < 0.5;
            selected = i;
            last_click = {now, twice ? -1 : i};
            if (twice)
                action('i');
        }
    } else if (mode == Mode::browser) {
        int i = browser.top + row - (PRow + 4);
        if (col > PCol && col < PCol + PCols - 1 && row >= PRow + 4 && row < PRow + 4 + ListRows && i < browser_items()) {
            bool twice = last_click.row == i && now - last_click.at < 0.5;
            browser.cursor = i;
            last_click = {now, twice ? -1 : i};
            if (twice)
                activate(i);
        }
    }
}

void wheel(int dy)
{
    if (mode == Mode::browser)
        browser.cursor = std::max(0, std::min(browser_items() - 1, browser.cursor - 3 * dy));
    else if (mode == Mode::panel)
        selected = std::max(0, selected - dy);
}

void draw(uint32_t *frame)
{
    fb = frame;
    if (mode == Mode::panel)
        draw_panel();
    else if (mode == Mode::browser)
        draw_browser();
    if (!toast.empty() && clock_seconds() < toast_until) {
        fill(0, Rows - 2, Cols, 1, Good);
        text(1, Rows - 2, head(toast, size_t(Cols - 2)), Fg, Good);
    }
    if (!hint_volume.empty()) {
        fill(0, Rows - 1, Cols, 1, Warn);
        text(1, Rows - 1, head("Please insert the disk " + hint_volume + ": drop its folder on this window, or press F12", size_t(Cols - 2)), Fg,
             Warn);
    }
}

} // namespace overlay

} // namespace amos::host

#pragma pack(pop)
