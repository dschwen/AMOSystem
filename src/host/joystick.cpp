// The joysticks of the Amiga's two ports (port 1: Joy(0), the mouse's port; port 2: Joy(1)), from
// the system's game controllers or the keyboard (the cursor keys, Control for fire). Each port has a
// source: "auto" (port 2 the first controller, port 1 the second one), "padN" (the Nth connected),
// "keyboard" or "none". The settings are amos.conf's [joystick] port1 / port2 (devices.cpp).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#pragma pack(push, 16)
namespace amos::host {

void joystick_port(int port, unsigned directions, bool fire);     // hal_chips.cpp

namespace devices {
const char *joystick_setting(int port);
void remember_joystick(int port, const char *source);
} // namespace devices

namespace joystick {

namespace {

constexpr unsigned Up = 1, Down = 2, Left = 4, Right = 8;

struct Pad {
    int id;                 // SDL's instance
    std::string name;
    unsigned buttons;       // the d-pad's directions
    unsigned stick;         // the left stick's
    bool fire[4];           // A, B, X, Y
};
std::vector<Pad> pads;      // in the order they were connected
unsigned keys;              // the cursor keys held
bool control[2];            // left, right Control
std::string source[2];
bool read;

void settings()
{
    if (read)
        return;
    read = true;
    for (int p = 0; p < 2; p++) {
        source[p] = devices::joystick_setting(p);
        if (source[p].empty())
            source[p] = "auto";
    }
}

const Pad *pad_of(int port)
{
    const std::string &s = source[port];
    size_t n = 0;
    if (s == "auto")
        n = port == 1 ? 0 : 1;
    else if (s.compare(0, 3, "pad") == 0 && s.size() > 3)
        n = size_t(atoi(s.c_str() + 3) - 1);
    else
        return nullptr;
    return n < pads.size() ? &pads[n] : nullptr;
}

void update()
{
    settings();
    for (int port = 0; port < 2; port++) {
        unsigned directions = 0;
        bool fire = false;
        if (source[port] == "keyboard") {
            directions = keys;
            fire = control[0] || control[1];
        } else if (const Pad *p = pad_of(port)) {
            directions = p->buttons | p->stick;
            fire = p->fire[0] || p->fire[1] || p->fire[2] || p->fire[3];
        }
        // (both ways of a direction at once: neither)
        if ((directions & (Up | Down)) == (Up | Down))
            directions &= ~(Up | Down);
        if ((directions & (Left | Right)) == (Left | Right))
            directions &= ~(Left | Right);
        joystick_port(port, directions, fire);
    }
}

Pad *find(int id)
{
    for (Pad &p : pads)
        if (p.id == id)
            return &p;
    return nullptr;
}

} // namespace

void added(int id, const char *name)
{
    if (find(id))
        return;
    pads.push_back({id, name ? name : "Game controller", 0, 0, {}});
    update();
}

void removed(int id)
{
    for (size_t i = 0; i < pads.size(); i++)
        if (pads[i].id == id) {
            pads.erase(pads.begin() + long(i));
            break;
        }
    update();
}

// SDL's game controller buttons: A, B, X, Y fire; the d-pad (11-14) the directions
void button(int id, int number, bool down)
{
    Pad *p = find(id);
    if (!p)
        return;
    static const unsigned dpad[] = {Up, Down, Left, Right};
    if (number >= 0 && number < 4)
        p->fire[number] = down;
    else if (number >= 11 && number <= 14)
        p->buttons = down ? p->buttons | dpad[number - 11] : p->buttons & ~dpad[number - 11];
    else
        return;
    update();
}

// The left stick (axes 0 and 1), past a third of its way
void axis(int id, int number, int value)
{
    Pad *p = find(id);
    if (!p || number > 1)
        return;
    constexpr int Threshold = 12000;
    unsigned less = number ? Up : Left, more = number ? Down : Right;
    p->stick &= ~(less | more);
    if (value < -Threshold)
        p->stick |= less;
    else if (value > Threshold)
        p->stick |= more;
    update();
}

// The keyboard's joystick (a port set to "keyboard"): the cursor keys and either Control. The keys
// still go to AMOS as well.
void key(int scancode, bool down)
{
    settings();
    if (source[0] != "keyboard" && source[1] != "keyboard")
        return;
    unsigned bit = 0;
    switch (scancode) {
    case 82: bit = Up; break;
    case 81: bit = Down; break;
    case 80: bit = Left; break;
    case 79: bit = Right; break;
    case 224: control[0] = down; break;
    case 228: control[1] = down; break;
    default: return;
    }
    if (bit)
        keys = down ? keys | bit : keys & ~bit;
    update();
}

// The source of a port, as the Devices panel shows it
std::string describe(int port)
{
    settings();
    const std::string &s = source[port];
    if (s == "none")
        return "none";
    if (s == "keyboard")
        return "keyboard (cursor keys, Ctrl)";
    const Pad *p = pad_of(port);
    std::string what = p ? p->name : "no controller";
    return s == "auto" ? "auto: " + what : what;
}

// The next source of a port: auto, none, keyboard, each controller connected
void cycle(int port)
{
    settings();
    std::vector<std::string> choices = {"auto", "none", "keyboard"};
    for (size_t i = 0; i < pads.size(); i++)
        choices.push_back("pad" + std::to_string(i + 1));
    size_t at = 0;
    for (size_t i = 0; i < choices.size(); i++)
        if (choices[i] == source[port])
            at = i;
    source[port] = choices[(at + 1) % choices.size()];
    devices::remember_joystick(port, source[port].c_str());
    keys = 0;
    update();
}

} // namespace joystick
} // namespace amos::host
#pragma pack(pop)
