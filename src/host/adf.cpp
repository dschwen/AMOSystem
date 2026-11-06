// A floppy image's files, unpacked into a folder: see adf.h.

#include "host/adf.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <utime.h>

#include "host/windows_crt.h"

#if defined(_WIN32)
#include "host/windows_platform.h"
#define mkdir(path, mode) amos::host::windows::make_directory(path)
#endif

namespace {

constexpr uint32_t Block = 512, Hash = 72;
constexpr uint32_t TypeHeader = 2, TypeData = 8, TypeList = 16;
constexpr int32_t SecRoot = 1, SecDir = 2, SecFile = -3;
constexpr int64_t Epoch = 252460800;       // 1978-01-01
constexpr const char *OrderFile = ".amos-order";

struct Disk {
    uint8_t *data = nullptr;
    uint32_t blocks = 0, root = 0;
    bool ffs = false, intl = false;
};

uint32_t be32(const uint8_t *p) { return uint32_t(p[0]) << 24 | uint32_t(p[1]) << 16 | uint32_t(p[2]) << 8 | p[3]; }

const uint8_t *block(const Disk &d, uint32_t n) { return n < d.blocks ? d.data + size_t(n) * Block : nullptr; }

bool sum_ok(const uint8_t *b)
{
    uint32_t sum = 0;
    for (uint32_t i = 0; i < Block; i += 4)
        sum += be32(b + i);
    return sum == 0;
}

// A header or list block of the given type and secondary type, its checksum right
const uint8_t *checked(const Disk &d, uint32_t n, uint32_t type)
{
    const uint8_t *b = block(d, n);
    return b && be32(b) == type && sum_ok(b) ? b : nullptr;
}

int32_t secondary(const uint8_t *b) { return int32_t(be32(b + Block - 4)); }

// The name of a header block (a length and up to 30 characters)
void name_of(const uint8_t *b, char *to)
{
    unsigned n = b[432] < 30 ? b[432] : 30;
    memcpy(to, b + 433, n);
    to[n] = 0;
}

bool open_disk(const char *path, Disk &d, const char *&message)
{
    FILE *f = fopen(path, "rb");
    if (!f) {
        message = "cannot read the image";
        return false;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    // 880 KiB (DD) or 1760 KiB (HD)
    if (size != 901120 && size != 1802240) {
        fclose(f);
        message = "not a floppy image (880 or 1760 KiB)";
        return false;
    }
    d.data = static_cast<uint8_t *>(malloc(size_t(size)));
    bool read = d.data && fread(d.data, 1, size_t(size), f) == size_t(size);
    fclose(f);
    if (!read) {
        free(d.data);
        d.data = nullptr;
        message = "cannot read the image";
        return false;
    }
    d.blocks = uint32_t(size / Block);
    d.root = d.blocks / 2;
    if (memcmp(d.data, "DOS", 3) || d.data[3] > 7) {
        message = "not an AmigaDOS disk (no DOS boot block)";
        return false;
    }
    d.ffs = d.data[3] & 1;
    d.intl = (d.data[3] & 6) != 0;
    const uint8_t *r = checked(d, d.root, TypeHeader);
    if (!r || secondary(r) != SecRoot) {
        message = "the disk has no valid root block";
        return false;
    }
    return true;
}

void close_disk(Disk &d)
{
    free(d.data);
    d.data = nullptr;
}

time_t date_of(const uint8_t *b)
{
    return time_t(Epoch + int64_t(be32(b + 420)) * 86400 + int64_t(be32(b + 424)) * 60 + be32(b + 428) / 50);
}

// A file's contents from its header block into path: false if a block was bad
bool dump_file(const Disk &d, const uint8_t *header, const char *path)
{
    FILE *f = fopen(path, "wb");
    if (!f)
        return false;
    uint32_t size = be32(header + 324), left = size;
    bool good = true;
    const uint8_t *h = header;
    for (uint32_t guard = 0; h && guard <= d.blocks; guard++) {
        uint32_t count = be32(h + 8);
        if (count > Hash) {
            good = false;
            break;
        }
        for (uint32_t i = 0; i < count && left; i++) {
            const uint8_t *b = block(d, be32(h + 24 + 4 * (Hash - 1 - i)));
            uint32_t length = d.ffs ? Block : Block - 24;
            const uint8_t *from = d.ffs ? b : b ? b + 24 : nullptr;
            if (!b || (!d.ffs && (be32(b) != TypeData || !sum_ok(b) || be32(b + 12) > Block - 24))) {
                good = false;
                left = 0;
                break;
            }
            if (!d.ffs)
                length = be32(b + 12);
            if (length > left)
                length = left;
            if (fwrite(from, 1, length, f) != length)
                good = false;
            left -= length;
        }
        uint32_t next = be32(h + 504);
        if (!next || !left)
            break;
        h = checked(d, next, TypeList);
        if (h && be32(h + 4) != next)
            h = nullptr;
        if (!h)
            good = false;
    }
    if (left)
        good = false;
    fclose(f);
    return good;
}

// The entries of directory block dir (the hash table's order) into path
bool dump_dir(const Disk &d, uint32_t dir, const char *path, int depth, int &bad)
{
    const uint8_t *b = checked(d, dir, TypeHeader);
    if (!b || depth > 64) {
        bad++;
        return false;
    }
    uint32_t *list = static_cast<uint32_t *>(malloc(sizeof(uint32_t) * (d.blocks + 1)));
    if (!list)
        return false;
    uint32_t n = 0;
    for (uint32_t i = 0; i < Hash; i++)
        for (uint32_t e = be32(b + 24 + 4 * i), guard = 0; e && guard <= d.blocks && n < d.blocks; guard++) {
            list[n++] = e;
            const uint8_t *x = block(d, e);
            e = x ? be32(x + 496) : 0;
        }
    // The disk's order of the entries is written in the folder (host_files reads it: the
    // host's own order is by time, which the file system's clock makes arbitrary)
    char order[1100];
    snprintf(order, sizeof order, "%s/%s", path, OrderFile);
    FILE *list_file = fopen(order, "wb");
    for (uint32_t i = 0; i < n; i++) {
        const uint8_t *e = checked(d, list[i], TypeHeader);
        char name[32];
        if (list_file && e && (secondary(e) == SecDir || secondary(e) == SecFile)) {
            name_of(e, name);
            fprintf(list_file, "%s\n", name);
        }
    }
    if (list_file)
        fclose(list_file);
    for (uint32_t i = n; i-- > 0;) {
        const uint8_t *e = checked(d, list[i], TypeHeader);
        if (!e || (secondary(e) != SecDir && secondary(e) != SecFile)) {
            bad++;
            continue;
        }
        char name[32], full[1024];
        name_of(e, name);
        if (!*name || strchr(name, '/') || snprintf(full, sizeof full, "%s/%s", path, name) >= int(sizeof full)) {
            bad++;
            continue;
        }
        if (secondary(e) == SecDir) {
            if (mkdir(full, 0777) != 0 && errno != EEXIST) {
                bad++;
                continue;
            }
            dump_dir(d, list[i], full, depth + 1, bad);
        } else if (!dump_file(d, e, full)) {
            bad++;
        }
        struct utimbuf t = {date_of(e), date_of(e)};
        utime(full, &t);
    }
    free(list);
    return true;
}

} // namespace

namespace amos::host::adf {

bool label_of(const char *image, char *label, size_t size)
{
    FILE *f = fopen(image, "rb");
    if (!f)
        return false;
    fseek(f, 0, SEEK_END);
    long total = ftell(f);
    bool ok = total == 901120 || total == 1802240;
    uint8_t boot[4], root[Block];
    if (ok) {
        fseek(f, 0, SEEK_SET);
        ok = fread(boot, 1, 4, f) == 4 && !memcmp(boot, "DOS", 3);
    }
    if (ok) {
        fseek(f, long(total / 2), SEEK_SET);
        ok = fread(root, 1, Block, f) == Block && be32(root) == TypeHeader && sum_ok(root) && secondary(root) == SecRoot;
    }
    fclose(f);
    if (!ok)
        return false;
    char name[32];
    name_of(root, name);
    snprintf(label, size, "%s", name);
    return true;
}

bool unpack(const char *image, const char *folder, char *label, size_t size, int &bad, const char *&message)
{
    Disk d;
    bad = 0;
    message = "";
    if (!open_disk(image, d, message)) {
        close_disk(d);
        return false;
    }
    const uint8_t *r = block(d, d.root);
    char name[32];
    name_of(r, name);
    snprintf(label, size, "%s", name);
    if (mkdir(folder, 0777) != 0 && errno != EEXIST) {
        close_disk(d);
        message = "cannot make the folder";
        return false;
    }
    dump_dir(d, d.root, folder, 0, bad);
    struct utimbuf t = {date_of(r), date_of(r)};
    utime(folder, &t);
    close_disk(d);
    return true;
}

} // namespace amos::host::adf
