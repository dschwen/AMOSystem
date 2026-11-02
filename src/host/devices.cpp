// The host's devices: see devices.h and docs/DEVICES.md.
//
// A table of names (assigns, and volumes: the disks), the units that name
// some of the volumes (DF0:...DF3: floppies, DH0:...DH9: hard disks), the
// devices directory (each folder in it a volume, mounted at once) and the
// library (folders that are disks, inserted when a program asks for their
// label). Set up by amos.conf (INI), the command line and the environment.

#include "host/devices.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>

#if !defined(_WIN32)
#include <ftw.h>
#endif

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

// (No std::filesystem and no std::error_code here, or anywhere in the host: the host is built with
// -fpack-struct=2 for the Amiga's structures, which changes their layout in our code but not
// inside libstdc++.so, and a call across would write over the stack.)

#include "host/windows_crt.h"

#if defined(_WIN32)
#include "host/windows_platform.h"
#define realpath(path, ignored) amos::host::windows::canonical_path(path)
#define mkdir(path, mode) amos::host::windows::make_directory(path)
#endif

namespace amos::host {
void disk_event(bool inserted);       // sdl.cpp: the input event DISKINSERTED / DISKREMOVED
}

namespace amos::host::devices {


namespace {

struct Volume {
    std::string name, path;
    bool volume = false;        // a disk (a label), not an assign
    bool read_only = false;
    bool user = false;          // set up by the user (the configuration, the command line, the panel)
};
std::vector<Volume> table;      // (a removed assign keeps its place, its name empty)

int unit_medium[Units];         // the volume in a unit, -1 none
bool unit_set[Units];           // a floppy unit the configuration names
int floppy_drives = 2;

struct Entry {
    std::string path, label;
    bool read_only = false, set = false;
};

struct Config {
    int floppy_drives = -1;
    std::string devices_dir;
    std::vector<std::string> library;
    Entry unit[Units];
    std::vector<std::pair<std::string, std::string>> assigns;
};

std::vector<std::pair<std::string, std::string>> options;     // the command line's
std::string config_file, program_path;
std::vector<std::string> library;
std::string devices_root;
std::string ram_path;
std::vector<std::string> ejected;       // folders the user took out: not mounted again by a scan
bool initialised;

// ---- names -----------------------------------------------------------------

std::string unit_name(int u)
{
    char text[8];
    snprintf(text, sizeof text, u < Floppies ? "DF%d" : "DH%d", u < Floppies ? u : u - Floppies);
    return text;
}

// DF0... / DH0... (n characters): its number in the units, or -1
int parse_unit(const char *name, size_t n)
{
    if (n != 3 || toupper(static_cast<unsigned char>(name[0])) != 'D' || name[2] < '0' || name[2] > '9')
        return -1;
    int k = name[2] - '0';
    switch (toupper(static_cast<unsigned char>(name[1]))) {
    case 'F': return k < Floppies ? k : -1;
    case 'H': return Floppies + k;
    }
    return -1;
}

bool unit_exists(int u) { return u < Floppies ? u < floppy_drives || unit_set[u] || unit_medium[u] >= 0 : unit_medium[u] >= 0; }

bool same(const std::string &a, const char *b, size_t n) { return a.size() == n && !strncasecmp(a.c_str(), b, n); }

int find(const char *name, size_t n, bool volumes)
{
    for (size_t i = 0; i < table.size(); i++)
        if (table[i].volume == volumes && !table[i].name.empty() && same(table[i].name, name, n))
            return int(i);
    return -1;
}

std::string real_path(const std::string &path)
{
    char *real = realpath(path.c_str(), nullptr);
    std::string out = real ? real : path;
    free(real);
    return out;
}

bool is_directory(const std::string &path)
{
    struct stat st;
    return stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

std::string base_name(std::string path)
{
    while (path.size() > 1 && path.back() == '/')
        path.pop_back();
    size_t slash = path.rfind('/');
    return slash == std::string::npos ? path : path.substr(slash + 1);
}

std::string trim(const std::string &s)
{
    size_t a = 0, b = s.size();
    while (a < b && isspace(static_cast<unsigned char>(s[a])))
        a++;
    while (b > a && isspace(static_cast<unsigned char>(s[b - 1])))
        b--;
    return s.substr(a, b - a);
}

std::string lower(std::string s)
{
    for (char &c : s)
        c = char(tolower(static_cast<unsigned char>(c)));
    return s;
}

void warn(const std::string &text) { fprintf(stderr, "amos: %s\n", text.c_str()); }

// ---- the configuration -------------------------------------------------------

// A path of a configuration: ~ for the home directory, relative to the folder of the file
std::string expand(const std::string &value, const std::string &base)
{
    if (value.empty())
        return value;
    if (value[0] == '~' && (value.size() == 1 || value[1] == '/')) {
        const char *home = getenv("HOME");
        return std::string(home ? home : ".") + value.substr(1);
    }
#if defined(_WIN32)
    if (value.size() > 2 && value[1] == ':' && (value[2] == '/' || value[2] == '\\'))
        return value;
#endif
    if (value[0] == '/' || base.empty())
        return value;
    return base + "/" + value;
}

bool truth(const std::string &v)
{
    std::string s = lower(v);
    return s == "yes" || s == "true" || s == "on" || s == "1";
}

void set_entry(Config &c, int u, const std::string &key, const std::string &value, const std::string &base)
{
    Entry &e = c.unit[u];
    if (key == "path") {
        e.path = expand(value, base);
        e.set = true;
    } else if (key == "label") {
        e.label = value;
    } else if (key == "readonly") {
        e.read_only = truth(value);
    } else if (key == "image") {
        warn(unit_name(u) + ": disk images (" + value + ") are not supported yet");
    } else {
        warn("amos.conf: " + unit_name(u) + ": unknown key " + key);
    }
}

// An INI file: [section], key = value, # comments; false if it is not there
bool load(const std::string &file, Config &c)
{
    FILE *f = fopen(file.c_str(), "r");
    if (!f)
        return false;
    std::string base = file.substr(0, file.rfind('/') == std::string::npos ? 0 : file.rfind('/'));
    if (file.find('/') == std::string::npos)
        base = ".";
    std::string section;
    char line[2048];
    while (fgets(line, sizeof line, f)) {
        std::string s = trim(line);
        if (s.empty() || s[0] == '#' || s[0] == ';')
            continue;
        if (s[0] == '[' && s.back() == ']') {
            section = lower(trim(s.substr(1, s.size() - 2)));
            continue;
        }
        size_t eq = s.find('=');
        if (eq == std::string::npos) {
            warn(file + ": not a setting: " + s);
            continue;
        }
        std::string key = trim(s.substr(0, eq)), value = trim(s.substr(eq + 1));
        int u = parse_unit(section.c_str(), section.size());
        if (u >= 0) {
            set_entry(c, u, lower(key), value, base);
        } else if (section == "assign") {
            c.assigns.push_back({key, expand(value, base)});
        } else if (section == "library" && lower(key) == "path") {
            c.library.push_back(expand(value, base));
        } else if (section == "devices" && lower(key) == "directory") {
            c.devices_dir = expand(value, base);
        } else if (section == "devices" && lower(key) == "floppy_drives") {
            c.floppy_drives = std::min(Floppies, std::max(0, atoi(value.c_str())));
        } else {
            warn(file + ": unknown setting [" + section + "] " + key);
        }
    }
    fclose(f);
    return true;
}

std::string user_config()
{
    if (const char *env = getenv("AMOS_CONFIG"); env && *env)
        return env;
#if defined(_WIN32)
    char *path = windows::user_config_path();
    std::string result = path ? path : "amos.conf";
    free(path);
    return result;
#else
    const char *xdg = getenv("XDG_CONFIG_HOME"), *home = getenv("HOME");
    if (xdg && *xdg)
        return std::string(xdg) + "/amos/amos.conf";
    return std::string(home ? home : ".") + "/.config/amos/amos.conf";
#endif
}

// "path[=label][,ro]": a disk of the command line
void apply_unit_option(Config &c, int u, const std::string &value)
{
    Entry &e = c.unit[u];
    std::string v = value;
    if (v.size() > 3 && v.compare(v.size() - 3, 3, ",ro") == 0) {
        e.read_only = true;
        v.resize(v.size() - 3);
    }
    size_t eq = v.rfind('=');
    if (eq != std::string::npos) {
        e.label = v.substr(eq + 1);
        v.resize(eq);
    }
    e.path = expand(v, "");
    e.set = true;
}

void apply_option(Config &c, const std::string &name, const std::string &value)
{
    int u = parse_unit(name.c_str(), name.size());
    if (u >= 0) {
        apply_unit_option(c, u, value);
    } else if (name == "assign") {
        size_t eq = value.find('=');
        if (eq == std::string::npos)
            warn("--assign NAME=path");
        else
            c.assigns.push_back({value.substr(0, eq), expand(value.substr(eq + 1), "")});
    } else if (name == "library") {
        c.library.push_back(expand(value, ""));
    } else if (name == "devices") {
        c.devices_dir = expand(value, "");
    }
}

// ---- the table ---------------------------------------------------------------

int add(const std::string &name, const std::string &path, bool volume, bool read_only = false, bool user = false)
{
    Volume v;
    v.name = name;
    v.path = path;
    v.volume = volume;
    v.read_only = read_only;
    v.user = user;
    table.push_back(v);
    return int(table.size()) - 1;
}

int next_hard_unit()
{
    for (int u = Floppies; u < Units; u++)
        if (unit_medium[u] < 0 && !unit_set[u])
            return u;
    return -1;
}

// A folder that is a disk, in a unit (auto: the next free hard disk's)
void mount(const std::string &label, const std::string &folder, bool read_only, int unit, bool user = false)
{
    std::string real = real_path(folder);
    if (!is_directory(real)) {
        warn((unit >= 0 ? unit_name(unit) + ": " : "") + folder + " is not a folder");
        return;
    }
    if (find(label.c_str(), label.size(), true) >= 0) {
        warn((unit >= 0 ? unit_name(unit) + ": " : std::string()) + "the name " + label + ": is already in use");
        return;
    }
    int i = add(label, real, true, read_only, user);
    if (unit < 0)
        unit = next_hard_unit();
    if (unit >= 0)
        unit_medium[unit] = i;
}

// The folders of a directory that are not volumes yet become volumes (the
// first time in the order of their names)
void scan(const std::string &directory)
{
    if (directory.empty())
        return;
    DIR *d = opendir(directory.c_str());
    if (!d)
        return;
    std::vector<std::string> names;
    while (struct dirent *e = readdir(d))
        if (e->d_name[0] != '.')
            names.push_back(e->d_name);
    closedir(d);
    std::sort(names.begin(), names.end(), [](const std::string &a, const std::string &b) { return strcasecmp(a.c_str(), b.c_str()) < 0; });
    for (const std::string &name : names) {
        if (name.size() >= 64 || name.find(':') != std::string::npos || find(name.c_str(), name.size(), false) >= 0 ||
            find(name.c_str(), name.size(), true) >= 0)
            continue;
        std::string path = directory + "/" + name;
        if (std::find(ejected.begin(), ejected.end(), real_path(path)) != ejected.end())
            continue;
        if (is_directory(path))
            mount(name, path, false, -1);
    }
}

// The devices directory: AMOS_DEVICES, the configuration's, "devices" where
// AMOS was started if there is one, else the user's own. A new one has a
// first disk, Work:
void open_devices(const Config &c)
{
    char path[1024];
    const char *env = getenv("AMOS_DEVICES"), *start = getenv("AMOS_START_DIR");
    const char *home = getenv("HOME"), *data = getenv("XDG_DATA_HOME");
#if defined(_WIN32)
    if (!home) home = getenv("USERPROFILE");
    if (!data) data = getenv("LOCALAPPDATA");
#endif
    bool fresh = false;
    if (env && *env) {
        snprintf(path, sizeof path, "%s", env);
        fresh = mkdir(path, 0777) == 0;
    } else if (!c.devices_dir.empty()) {
        snprintf(path, sizeof path, "%s", c.devices_dir.c_str());
        fresh = mkdir(path, 0777) == 0;
    } else if (start && (snprintf(path, sizeof path, "%s/devices", start), is_directory(path))) {
        fresh = false;
    } else {
#if defined(_WIN32)
        char *user = windows::user_data_directory();
        if (!user) return;
        snprintf(path, sizeof path, "%s", user);
        free(user);
#else
        if (data && *data)
            snprintf(path, sizeof path, "%s/amos", data);
        else
            snprintf(path, sizeof path, "%s/.local/share/amos", home ? home : ".");
#endif
        for (char *s = path + 1; *s; s++)
            if (*s == '/') {
                *s = 0;
                mkdir(path, 0777);
                *s = '/';
            }
        mkdir(path, 0777);
        strncat(path, "/devices", sizeof path - strlen(path) - 1);
        fresh = mkdir(path, 0777) == 0;
    }
    char *real = realpath(path, nullptr);
    if (!real)
        return;
    devices_root = real;
    free(real);
    if (fresh)
        mkdir((devices_root + "/Work").c_str(), 0777);
}

// RAM: is empty at every start: a folder of its own, removed at the end
#if !defined(_WIN32)
int remove_entry(const char *path, const struct stat *, int, struct FTW *) { return remove(path); }
#endif

void remove_ram()
{
    // (only a folder this run made: its name has the mark)
    if (ram_path.size() > 10 && ram_path.find("/amos-ram-") != std::string::npos)
#if defined(_WIN32)
        windows::remove_tree(ram_path.c_str());
#else
        nftw(ram_path.c_str(), remove_entry, 16, FTW_DEPTH | FTW_PHYS);
#endif
}

void make_ram()
{
#if defined(_WIN32)
    char *created = windows::temporary_directory();
    if (!created) return;
    ram_path = created;
    free(created);
#else
    const char *tmp = getenv("TMPDIR");
    std::string pattern = std::string(tmp && *tmp ? tmp : "/tmp") + "/amos-ram-XXXXXX";
    std::vector<char> text(pattern.begin(), pattern.end());
    text.push_back(0);
    if (!mkdtemp(text.data()))
        return;
    ram_path = text.data();
#endif
    atexit(remove_ram);
    add("RAM", real_path(ram_path), true);
}

void init()
{
    if (initialised)
        return;
    initialised = true;
    for (int &m : unit_medium)
        m = -1;

    // the configuration: the user's, the program's, the command line's
    Config c;
    load(config_file.empty() ? user_config() : config_file, c);
    if (!program_path.empty()) {
        std::string profile = program_path;
        size_t dot = profile.rfind('.'), slash = profile.rfind('/');
        if (dot != std::string::npos && (slash == std::string::npos || dot > slash))
            profile.resize(dot);
        load(profile + ".amos.conf", c);
    }
    for (const auto &o : options)
        apply_option(c, o.first, o.second);
    if (c.floppy_drives >= 0)
        floppy_drives = c.floppy_drives;

    // the assigns: AMOS_ASSIGNS="NAME=directory;..."
    const char *env = getenv("AMOS_ASSIGNS");
    while (env && *env) {
        const char *eq = strchr(env, '='), *end = strchr(env, ';');
        if (!end)
            end = env + strlen(env);
        if (eq && eq < end && size_t(eq - env) < 64)
            add(std::string(env, size_t(eq - env)), std::string(eq + 1, size_t(end - eq - 1)), false);
        env = *end ? end + 1 : end;
    }
    for (Volume &v : table)
        v.path = real_path(v.path);
    // the start directory a volume (AMOSPRO:, unless an assign is it), so
    // that the paths stay as short as an Amiga's
    if (char *cwd = getcwd(nullptr, 0)) {
#if defined(_WIN32)
        for (char *p = cwd; *p; p++) if (*p == '\\') *p = '/';
#endif
        bool covered = false;
        for (const Volume &v : table)
            covered |= v.path == cwd;
        if (!covered)
            add("AMOSPRO", cwd, false);
        free(cwd);
    }
    // START: where AMOS was started (the programs named on the command line)
    if (const char *start = getenv("AMOS_START_DIR"); start && *start && find("START", 5, false) < 0)
        add("START", real_path(start), false);
    for (const auto &a : c.assigns)
        if (find(a.first.c_str(), a.first.size(), false) < 0)
            add(a.first, real_path(a.second), false);

    // the disks of the configuration
    for (int u = 0; u < Units; u++) {
        const Entry &e = c.unit[u];
        unit_set[u] = u < Floppies && e.set;
        if (!e.set)
            continue;
        mount(e.label.empty() ? base_name(e.path) : e.label, e.path, e.read_only, u, true);
    }
    // the devices directory's, the library, the volatile RAM:
    open_devices(c);
    scan(devices_root);
    library = c.library;
    for (std::string &l : library)
        l = real_path(l);
    if (find("RAM", 3, false) < 0 && find("RAM", 3, true) < 0)
        make_ram();
    // the folder of the program, a disk of the name of the folder
    if (const char *dir = getenv("AMOS_PROGRAM_DIR"); dir && *dir && is_directory(dir)) {
        std::string label = base_name(dir);
        if (!label.empty() && label != "/" && label.size() < 64 && label.find(':') == std::string::npos &&
            find(label.c_str(), label.size(), false) < 0 && find(label.c_str(), label.size(), true) < 0 &&
            parse_unit(label.c_str(), label.size()) < 0)
            add(label, real_path(dir), true);
    }
}

// A disk of the library with that label: a folder of its name, inserted
const char *from_library(const char *name, size_t n)
{
    for (const std::string &dir : library) {
        DIR *d = opendir(dir.c_str());
        if (!d)
            continue;
        std::string found;
        while (struct dirent *e = readdir(d))
            if (e->d_name[0] != '.' && strlen(e->d_name) == n && !strncasecmp(e->d_name, name, n))
                found = e->d_name;
        closedir(d);
        if (found.empty() || !is_directory(dir + "/" + found))
            continue;
        mount(found, dir + "/" + found, false, -1);
        int i = find(found.c_str(), found.size(), true);
        return i >= 0 ? table[size_t(i)].path.c_str() : nullptr;
    }
    return nullptr;
}

} // namespace

bool option(const char *name, const char *value)
{
    std::string n = lower(name);
    if (n == "config") {
        config_file = value;
        return true;
    }
    if (parse_unit(n.c_str(), n.size()) >= 0 || n == "assign" || n == "library" || n == "devices") {
        options.push_back({n, value});
        return true;
    }
    return false;
}

void set_program(const char *path) { program_path = path; }

const char *resolve(const char *name, size_t n, Status &status)
{
    init();
    status = Status::found;
    for (int pass = 0; pass < 2; pass++) {
        int i = find(name, n, false);
        if (i < 0)
            i = find(name, n, true);
        if (i >= 0)
            return table[size_t(i)].path.c_str();
        if (!pass) {
            int u = parse_unit(name, n);
            if (u >= 0 && unit_exists(u)) {
                if (unit_medium[u] >= 0)
                    return table[size_t(unit_medium[u])].path.c_str();
                status = Status::no_disk;
                return nullptr;
            }
            // (a disk made since: in the devices directory)
            scan(devices_root);
        }
    }
    if (const char *path = from_library(name, n))
        return path;
    // RAM:'s volume, "Ram Disk"
    if (n == 8 && !strncasecmp(name, "Ram Disk", 8))
        return resolve("RAM", 3, status);
    status = Status::not_mounted;
    return nullptr;
}

bool exists(const char *name, size_t n)
{
    Status status;
    return resolve(name, n, status) || status == Status::no_disk;
}

const char *volume_of(const char *real)
{
    init();
    for (int volumes = 1; volumes >= 0; volumes--)
        for (const Volume &v : table)
            if (v.volume == bool(volumes) && !v.name.empty() && v.path == real)
                return strcasecmp(v.name.c_str(), "RAM") ? v.name.c_str() : "Ram Disk";
    return nullptr;
}

bool read_only(const char *path)
{
    init();
    size_t n = strlen(path);
    for (const Volume &v : table)
        if (v.read_only && n >= v.path.size() && !strncmp(path, v.path.c_str(), v.path.size()) &&
            (n == v.path.size() || path[v.path.size()] == '/'))
            return true;
    return false;
}

bool assign(const char *name, size_t n, const char *path)
{
    init();
    int i = find(name, n, false);
    if (!path) {
        if (i < 0)
            return false;
        table[size_t(i)].name.clear();
        return true;
    }
    if (i < 0) {
        if (n == 0 || n >= 64)
            return false;
        i = add(std::string(name, n), "", false);
    }
    table[size_t(i)].path = real_path(path);
    return true;
}

int unit_of(const char *name, size_t n) { return parse_unit(name, n); }

bool unit_info(int u, UnitInfo &info)
{
    init();
    if (u < 0 || u >= Units)
        return false;
    memset(&info, 0, sizeof info);
    snprintf(info.name, sizeof info.name, "%s", unit_name(u).c_str());
    info.exists = unit_exists(u);
    if (unit_medium[u] >= 0) {
        const Volume &v = table[size_t(unit_medium[u])];
        info.has_disk = true;
        info.read_only = v.read_only;
        info.saved = v.user;
        snprintf(info.label, sizeof info.label, "%s", v.name.c_str());
        snprintf(info.path, sizeof info.path, "%s", v.path.c_str());
    }
    return true;
}

namespace {

// The disk of a unit out (the folder is not mounted again by a scan)
void take_out(int u)
{
    Volume &v = table[size_t(unit_medium[u])];
    ejected.push_back(v.path);
    v.name.clear();
    unit_medium[u] = -1;
    disk_event(false);
}

} // namespace

bool insert(int u, const char *folder, const char *label_in, bool read_only, const char *&message)
{
    init();
    message = "";
    if (u < 0 || u >= Units) {
        message = "no such unit";
        return false;
    }
    std::string real = real_path(folder);
    if (!is_directory(real)) {
        message = "not a folder";
        return false;
    }
    std::string label = label_in && *label_in ? label_in : base_name(real);
    if (label.empty() || label.size() >= 64 || label.find(':') != std::string::npos || label.find('/') != std::string::npos) {
        message = "the name cannot be a disk's";
        return false;
    }
    // a name in use by another disk, or by an assign
    int other = find(label.c_str(), label.size(), true);
    if (other >= 0 && table[size_t(other)].path != real && other != unit_medium[u]) {
        message = "the name is already in use";
        return false;
    }
    if (find(label.c_str(), label.size(), false) >= 0 || parse_unit(label.c_str(), label.size()) >= 0) {
        message = "the name is already in use";
        return false;
    }
    // the same disk in another drive: it moves
    for (int x = 0; x < Units; x++)
        if (x != u && unit_medium[x] >= 0 && table[size_t(unit_medium[x])].path == real)
            take_out(x);
    if (unit_medium[u] >= 0)
        take_out(u);
    ejected.erase(std::remove(ejected.begin(), ejected.end(), real), ejected.end());
    // (the same label with its folder gone, if it is in the table dead: no)
    unit_medium[u] = add(label, real, true, read_only, true);
    if (u < Floppies)
        unit_set[u] = true;
    disk_event(true);
    return true;
}

int mount_folder(const char *folder, const char *label, bool read_only, const char *&message)
{
    init();
    int u = next_hard_unit();
    if (u < 0) {
        message = "all the hard disk units are in use";
        return -1;
    }
    return insert(u, folder, label, read_only, message) ? u : -1;
}

bool eject(int u)
{
    init();
    if (u < 0 || u >= Units || unit_medium[u] < 0)
        return false;
    take_out(u);
    return true;
}

bool set_read_only(int u, bool read_only)
{
    init();
    if (u < 0 || u >= Units || unit_medium[u] < 0)
        return false;
    table[size_t(unit_medium[u])].read_only = read_only;
    return true;
}

const char *config_path()
{
    static std::string path;
    path = config_file.empty() ? user_config() : config_file;
    return path.c_str();
}

int library_count()
{
    init();
    return int(library.size());
}

const char *library_path(int n)
{
    init();
    return n >= 0 && n < int(library.size()) ? library[size_t(n)].c_str() : "";
}

bool save_config(const char *&message)
{
    init();
    message = "";
    std::string file = config_path();
    // the rest of the file (its drives, library, assigns) as it is on disk
    Config c;
    load(file, c);
    std::string text = "# amos.conf: written by AMOS (the Devices panel). Edit it as you like.\n";
    if (c.floppy_drives >= 0 || !c.devices_dir.empty()) {
        text += "[devices]\n";
        if (c.floppy_drives >= 0)
            text += "floppy_drives = " + std::to_string(c.floppy_drives) + "\n";
        if (!c.devices_dir.empty())
            text += "directory = " + c.devices_dir + "\n";
    }
    if (!c.library.empty()) {
        text += "[library]\n";
        for (const std::string &l : c.library)
            text += "path = " + l + "\n";
    }
    for (int u = 0; u < Units; u++) {
        if (unit_medium[u] < 0 || !table[size_t(unit_medium[u])].user)
            continue;
        const Volume &v = table[size_t(unit_medium[u])];
        text += "[" + lower(unit_name(u)) + "]\npath = " + v.path + "\n";
        if (v.name != base_name(v.path))
            text += "label = " + v.name + "\n";
        if (v.read_only)
            text += "readonly = yes\n";
    }
    if (!c.assigns.empty()) {
        text += "[assign]\n";
        for (const auto &a : c.assigns)
            text += a.first + " = " + a.second + "\n";
    }
    // the folders above the file, the old file kept
    for (size_t i = file.find('/', 1); i != std::string::npos; i = file.find('/', i + 1))
        mkdir(file.substr(0, i).c_str(), 0777);
    rename(file.c_str(), (file + ".bak").c_str());
    FILE *f = fopen(file.c_str(), "w");
    if (!f) {
        message = strerror(errno);
        return false;
    }
    fwrite(text.data(), 1, text.size(), f);
    fclose(f);
    return true;
}

void each(void (*f)(const char *name, bool device, void *data), void *data)
{
    init();
    scan(devices_root);
    // (as the Amiga's lists: the devices (the units, RAM:) and the assigns, not the
    // labels of the disks in them)
    for (size_t i = 0; i < table.size(); i++) {
        if (table[i].name.empty() || (table[i].volume && strcasecmp(table[i].name.c_str(), "RAM")))
            continue;
        std::string name = table[i].name;
        f(name.c_str(), table[i].volume, data);
        // (RAM: is a device whose volume is "Ram Disk")
        if (!strcasecmp(name.c_str(), "RAM"))
            f("Ram Disk", false, data);
    }
    for (int u = 0; u < Units; u++)
        if (unit_exists(u))
            f(unit_name(u).c_str(), true, data);
}

} // namespace amos::host::devices
