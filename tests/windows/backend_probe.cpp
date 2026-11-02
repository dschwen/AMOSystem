// Exercise the actual normally packed Windows wrappers used by AMOS.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <string>
#include "host/windows_platform.h"
#include "host/windows_crt.h"

namespace {
using namespace amos::host::windows;
int failures;
void *caller;
int phase;
void check(bool ok, const char *name)
{
    printf("%s Windows backend: %s\n", ok ? "PASS" : "FAIL", name);
    if (!ok) failures++;
}
void task()
{
    alignas(16) jmp_buf jump;
    volatile int kept = 123;
    int code = setjmp(jump);
    if (!code) longjmp(jump, 19);
    check(code == 19 && kept == 123, "recovery on checked production fiber");
    phase = 1;
    fiber_switch(caller);
    check(kept == 123, "local survives production fiber yield");
    phase = 2;
    fiber_switch(caller);
    abort();
}
}

int backend_probe(void *main_fiber)
{
    check(image_below_4gb(), "complete executable image below 4GB");
    caller = main_fiber;
    void *fiber = fiber_create(256 * 1024, task);
    check(fiber != nullptr, "create checked production fiber");
    if (fiber) {
        fiber_switch(fiber);
        check(phase == 1, "first production yield");
        fiber_switch(fiber);
        check(phase == 2, "second production yield");
        fiber_delete(fiber);
    }
    uint64_t a = monotonic_microseconds();
    Sleep(2);
    check(monotonic_microseconds() > a, "monotonic clock advances");
    char *exe = executable_path();
    check(exe && !strchr(exe, '\\'), "executable path with normalized separators");
    free(exe);
    char *temp = temporary_directory();
    check(temp != nullptr, "create private RAM directory");
    if (temp) {
        std::string folder = std::string(temp) + "/nested földer";
        check(make_directory(folder.c_str()) == 0, "create directory with space");
        std::string one = folder + "/one-é.bin", two = folder + "/two.bin";
        const unsigned char data[] = {0, 10, 13, 26, 255};
        for (const auto &path : {one, two}) {
            FILE *f = fopen(path.c_str(), "wb");
            check(f != nullptr, "open binary file");
            if (f) { check(fwrite(data, 1, sizeof data, f) == sizeof data, "write binary bytes"); fclose(f); }
        }
        unsigned char got[sizeof data] = {};
        if (FILE *f = fopen(one.c_str(), "rb")) {
            check(fread(got, 1, sizeof got, f) == sizeof got && !memcmp(got, data, sizeof data),
                  "binary control characters round trip");
            fclose(f);
        } else check(false, "read binary file");
        uint64_t dev1, ino1, dev2, ino2;
        int64_t sec, ns;
        bool first = file_details(one.c_str(), &dev1, &ino1, &sec, &ns);
        bool second = file_details(two.c_str(), &dev2, &ino2, &sec, &ns);
        check(first && second && (dev1 != dev2 || ino1 != ino2), "distinct file identities");
        char *real = canonical_path(one.c_str());
        check(real && !strchr(real, '\\'), "canonical existing path");
        free(real);
        real = canonical_path((folder + "/missing").c_str());
        check(real == nullptr, "canonicalization rejects missing path");
        free(real);
        uint64_t total, avail;
        check(disk_space(one.c_str(), &total, &avail) && total > 0 && avail <= total,
              "disk space from regular file path");
        std::string source = std::string(temp) + "/resources", target = std::string(temp) + "/user runtime";
        check(make_directory(source.c_str()) == 0, "create immutable-resource fixture");
        check(make_directory((source+"/APSystem").c_str()) == 0, "create APSystem fixture");
        auto write = [](const std::string &p, const char *value) {
            FILE *f = fopen(p.c_str(), "wb");
            if (!f) return false;
            bool ok = fwrite(value, 1, strlen(value), f) == strlen(value);
            return fclose(f) == 0 && ok;
        };
        check(write(source+"/APSystem/AMOSPro_Editor_Config", "default"), "write default configuration");
        check(write(source+"/APSystem/resource.bin", "version-one"), "write initial resource");
        check(refresh_user_resources(source.c_str(), target.c_str()), "seed first-run user state");
        check(write(target+"/APSystem/AMOSPro_Editor_Config", "user preferences"), "modify user preference");
        check(write(source+"/APSystem/resource.bin", "version-two-longer"), "update installed resource");
        check(refresh_user_resources(source.c_str(), target.c_str()), "refresh upgraded resource tree");
        auto equals = [](const std::string &p, const char *value) {
            FILE *f = fopen(p.c_str(), "rb");
            if (!f) return false;
            char buf[64] = {}; size_t n = fread(buf, 1, sizeof buf, f); fclose(f);
            return n == strlen(value) && !memcmp(buf, value, n);
        };
        check(equals(target+"/APSystem/AMOSPro_Editor_Config", "user preferences"), "upgrade preserves user preferences");
        check(equals(target+"/APSystem/resource.bin", "version-two-longer"), "upgrade refreshes resource data");
        check(remove_tree(temp), "recursive private RAM cleanup");
        real = canonical_path(temp);
        check(real == nullptr, "RAM directory removed");
        free(real);
        free(temp);
    }
    return failures;
}
