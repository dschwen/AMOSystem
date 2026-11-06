// What is dropped on the window (or faked by AMOS_DROP, for the tests): a
// folder becomes a hard disk, "df0=folder" puts it in that unit, "df0="
// takes the disk out. A message tells what happened (host/overlay.cpp shows
// it in the window; here, and without a window, on stderr).
//
//   AMOS_DROP="SECONDS:SPEC,SECONDS:SPEC..."    the drops at those times after the first frame

#include "host/devices.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <time.h>

#include "host/cxx.h"

#if defined(_WIN32)
#include "host/windows_platform.h"
#endif

#include "host/windows_crt.h"

// Private host objects containing native C++ library members.
#pragma pack(push, 16)
namespace amos::host {

void message(const char *text);     // overlay.cpp

namespace {

struct Fake {
    double at;
    std::string spec;
    bool done;
};
std::vector<Fake> fakes;
bool parsed;
double started = -1;

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

void parse()
{
    parsed = true;
    const char *env = getenv("AMOS_DROP");
    while (env && *env) {
        const char *end = strchr(env, ','), *colon = strchr(env, ':');
        if (!end)
            end = env + strlen(env);
        if (colon && colon < end)
            fakes.push_back({atof(env), std::string(colon + 1, size_t(end - colon - 1)), false});
        env = *end ? end + 1 : end;
    }
}

} // namespace

// A path dropped: a folder is a hard disk, "df0=path" a disk in that unit, "df0=" takes it out
void drop(const char *spec)
{
    std::string text = spec;
    const char *reason = "";
    const char *eq = strchr(spec, '=');
    int unit = -1;
    if (eq && devices::unit_of(spec, size_t(eq - spec)) >= 0) {
        unit = devices::unit_of(spec, size_t(eq - spec));
        text = eq + 1;
    }
    char note[1200];
    if (unit >= 0 && text.empty()) {
        devices::UnitInfo before;
        devices::unit_info(unit, before);
        if (devices::eject(unit))
            snprintf(note, sizeof note, "%s: %s taken out", before.name, before.label);
        else
            snprintf(note, sizeof note, "%s: no disk to take out", before.name);
        message(note);
        return;
    }
    struct stat st;
    if (stat(text.c_str(), &st) != 0) {
        snprintf(note, sizeof note, "%s: not found", text.c_str());
    } else if (!S_ISDIR(st.st_mode)) {
        size_t n = text.size();
        if (n > 4 && !strcasecmp(text.c_str() + n - 4, ".adf")) {
            // a floppy image: into the unit (DF0: if none is named), write-protected
            int u = unit >= 0 ? unit : 0;
            devices::UnitInfo info;
            if (devices::insert(u, text.c_str(), nullptr, true, reason) && devices::unit_info(u, info))
                snprintf(note, sizeof note, "%s: %s", info.name, info.label);
            else
                snprintf(note, sizeof note, "%s: %s", text.c_str(), reason);
        } else {
            snprintf(note, sizeof note, "Only folders and disk images (.adf) can be disks: %s", text.c_str());
        }
    } else {
        int u = unit >= 0 ? (devices::insert(unit, text.c_str(), nullptr, false, reason) ? unit : -1)
                          : devices::mount_folder(text.c_str(), nullptr, false, reason);
        devices::UnitInfo info;
        if (u >= 0 && devices::unit_info(u, info))
            snprintf(note, sizeof note, "%s: %s", info.name, info.label);
        else
            snprintf(note, sizeof note, "%s: %s", text.c_str(), reason);
    }
    message(note);
}

// The faked drops that are due (called every frame)
void drops_frame()
{
    if (!parsed)
        parse();
    if (fakes.empty())
        return;
    double now = clock_seconds();
    if (started < 0)
        started = now;
    for (Fake &f : fakes)
        if (!f.done && now - started >= f.at) {
            f.done = true;
            drop(f.spec.c_str());
        }
}

// Is a faked drop still to come (a requester may wait for it with no window)
bool drops_pending()
{
    if (!parsed)
        parse();
    for (const Fake &f : fakes)
        if (!f.done)
            return true;
    return false;
}

} // namespace amos::host

#pragma pack(pop)
