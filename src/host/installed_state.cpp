// Native POSIX filesystem ABI: compile without the guest's -fpack-struct=2.
#if !defined(_WIN32)
#include "host/installed_state.h"
#include <filesystem>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>

namespace amos::host {
namespace fs = std::filesystem;
namespace {
bool retained(const fs::path &relative)
{
    auto s = relative.generic_string();
    auto ext = relative.extension().string();
    for (char &c : ext) if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
    return s == "AMOSPro_Interpreter_Config" || s == "APSystem/AMOSPro_Editor_Config" ||
           s == "APSystem/AMOSPro_Editor_Macros" || ext == ".amos";
}
bool seed(const fs::path &from, const fs::path &to, const fs::path &relative)
{
    std::error_code ec;
    if (fs::is_symlink(fs::symlink_status(from, ec))) return false;
    ec.clear();
    if (fs::is_symlink(fs::symlink_status(to, ec))) return false;
    ec.clear(); fs::create_directories(to, ec); if (ec) return false;
    fs::directory_iterator it(from, ec), end;
    while (!ec && it != end) {
        auto source = it->path(), target = to / source.filename(), rel = relative / source.filename();
        auto status = it->symlink_status(ec); if (ec || fs::is_symlink(status)) return false;
        if (fs::is_directory(status)) {
            if (!seed(source, target, rel)) return false;
        } else if (fs::is_regular_file(status)) {
            auto dest_status = fs::symlink_status(target, ec);
            if (ec && ec != std::errc::no_such_file_or_directory) return false;
            ec.clear();
            if (fs::is_symlink(dest_status) || fs::is_directory(dest_status)) return false;
            if (!fs::exists(dest_status) || !retained(rel)) {
                // Copy/rename atomically under the per-user lock. Do not follow
                // a pre-existing temporary symlink or touch unowned user files.
                auto temp = target; temp += ".amos-update-" + std::to_string(getpid());
                fs::remove(temp, ec); if (ec) return false;
                fs::copy_file(source, temp, fs::copy_options::none, ec); if (ec) return false;
                fs::rename(temp, target, ec); if (ec) { std::error_code ignored; fs::remove(temp, ignored); return false; }
            }
        } else return false;
        it.increment(ec);
    }
    return !ec;
}
}
bool refresh_posix_resources(const char *resources, const char *destination)
{
    return seed(fs::path(resources), fs::path(destination), {});
}
char *prepare_macos_state(const char *resources)
{
    const char *home = getenv("HOME");
    if (!home || !*home) return nullptr;
    auto root = fs::path(home) / "Library/Application Support/AMOSystem";
    std::error_code ec; fs::create_directories(root, ec); if (ec) return nullptr;
    int fd = open((root / "runtime.lock").c_str(), O_CREAT | O_RDWR | O_NOFOLLOW, 0600);
    if (fd < 0) return nullptr;
    if (flock(fd, LOCK_EX) != 0) { close(fd); return nullptr; }
    auto runtime = root / "runtime";
    bool ok = refresh_posix_resources(resources, runtime.c_str());
    flock(fd, LOCK_UN); close(fd);
    if (!ok) { fprintf(stderr, "amos: could not prepare writable macOS resources\n"); return nullptr; }
    return strdup(runtime.c_str());
}
}
#endif
