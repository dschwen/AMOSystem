// This translation unit must use normal Windows structure packing.
#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0602
#endif
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <shlobj.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include "host/windows_platform.h"
#define AMOS_WINDOWS_CRT_IMPL
#include "host/windows_crt.h"
#include <fcntl.h>
#include <direct.h>

namespace amos::host::windows {
namespace {
std::wstring wide(const char *path)
{
    int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, nullptr, 0);
    if (!n) { errno = EINVAL; return {}; }
    std::vector<wchar_t> buffer(size_t(n), L'\0');
    if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, buffer.data(), n))
        return {};
    for (wchar_t &c : buffer)
        if (c == L'/') c = L'\\';
    return buffer.data();
}
char *utf8(const wchar_t *path)
{
    int n = WideCharToMultiByte(CP_UTF8, 0, path, -1, nullptr, 0, nullptr, nullptr);
    char *text = n ? static_cast<char *>(malloc(size_t(n))) : nullptr;
    if (!text) { errno = ENOMEM; return nullptr; }
    if (!WideCharToMultiByte(CP_UTF8, 0, path, -1, text, n, nullptr, nullptr)) {
        free(text); errno = EINVAL; return nullptr;
    }
    for (char *p = text; *p; p++)
        if (*p == '\\') *p = '/';
    return text;
}
void error()
{
    switch (GetLastError()) {
    case ERROR_FILE_NOT_FOUND: case ERROR_PATH_NOT_FOUND: errno = ENOENT; break;
    case ERROR_ALREADY_EXISTS: case ERROR_FILE_EXISTS: errno = EEXIST; break;
    case ERROR_ACCESS_DENIED: errno = EACCES; break;
    case ERROR_SHARING_VIOLATION: errno = EBUSY; break;
    case ERROR_DIR_NOT_EMPTY: errno = ENOTEMPTY; break;
    case ERROR_DISK_FULL: errno = ENOSPC; break;
    default: errno = EIO; break;
    }
}
bool remove_wide(const std::wstring &path)
{
    DWORD attributes = GetFileAttributesW(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) { error(); return false; }
    if (!(attributes & FILE_ATTRIBUTE_DIRECTORY))
        return DeleteFileW(path.c_str()) != 0;
    // Never recurse through junctions/symlinks out of the private RAM tree.
    if (!(attributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
        WIN32_FIND_DATAW data;
        HANDLE find = FindFirstFileW((path + L"\\*").c_str(), &data);
        if (find == INVALID_HANDLE_VALUE) { error(); return false; }
        bool ok = true;
        do {
            if (wcscmp(data.cFileName, L".") && wcscmp(data.cFileName, L"..") &&
                !remove_wide(path + L"\\" + data.cFileName)) { ok = false; break; }
        } while (FindNextFileW(find, &data));
        DWORD last = GetLastError();
        FindClose(find);
        if (!ok || last != ERROR_NO_MORE_FILES) return false;
    }
    if (!RemoveDirectoryW(path.c_str())) { error(); return false; }
    return true;
}
void require_low_stack()
{
    ULONG_PTR low, high;
    GetCurrentThreadStackLimits(&low, &high);
    if (!low || low >= high || high > (uintptr_t(1) << 32)) {
        fprintf(stderr, "amos: Windows fiber stack [%p, %p) exceeds 32-bit address range\n",
                reinterpret_cast<void *>(low), reinterpret_cast<void *>(high));
        exit(70);
    }
}
struct FiberStart { void (*entry)(); };
VOID WINAPI fiber_entry(void *argument)
{
    require_low_stack();
    auto *start = static_cast<FiberStart *>(argument);
    auto entry = start->entry;
    delete start;
    entry();
    fprintf(stderr, "amos: unexpected return from Windows fiber\n");
    exit(70);
}
}
namespace {
std::wstring state_root()
{
    PWSTR folder = nullptr;
    if (FAILED(SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_CREATE, nullptr, &folder))) {
        errno = EIO; return {};
    }
    std::wstring root = std::wstring(folder) + L"\\AMOSystem";
    CoTaskMemFree(folder);
    if (!CreateDirectoryW(root.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) {
        error(); return {};
    }
    return root;
}
bool user_owned(const std::wstring &relative)
{
    // Shipped BASIC scripts may have been edited by the user, too.
    return (relative.size() >= 5 && !_wcsicmp(relative.c_str() + relative.size() - 5, L".AMOS")) ||
           !_wcsicmp(relative.c_str(), L"AMOSPro_Interpreter_Config") ||
           !_wcsicmp(relative.c_str(), L"APSystem\\AMOSPro_Editor_Config") ||
           !_wcsicmp(relative.c_str(), L"APSystem\\AMOSPro_Editor_Macros");
}
bool seed(const std::wstring &source, const std::wstring &dest, const std::wstring &relative = L"")
{
    if (!CreateDirectoryW(dest.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) {
        error(); return false;
    }
    WIN32_FIND_DATAW data;
    HANDLE find = FindFirstFileW((source + L"\\*").c_str(), &data);
    if (find == INVALID_HANDLE_VALUE) { error(); return false; }
    bool ok = true;
    do {
        if (!wcscmp(data.cFileName, L".") || !wcscmp(data.cFileName, L"..")) continue;
        std::wstring from = source + L"\\" + data.cFileName;
        std::wstring to = dest + L"\\" + data.cFileName;
        std::wstring rel = relative.empty() ? data.cFileName : relative + L"\\" + data.cFileName;
        if (data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) { errno = EINVAL; ok = false; break; }
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (!seed(from, to, rel)) { ok = false; break; }
        } else {
            WIN32_FILE_ATTRIBUTE_DATA existing;
            bool exists = GetFileAttributesExW(to.c_str(), GetFileExInfoStandard, &existing) != 0;
            if (exists && user_owned(rel)) continue;
            if (exists && existing.nFileSizeLow == data.nFileSizeLow && existing.nFileSizeHigh == data.nFileSizeHigh &&
                !CompareFileTime(&existing.ftLastWriteTime, &data.ftLastWriteTime)) continue;
            std::wstring temporary = to + L".seed-" + std::to_wstring(GetCurrentProcessId());
            if (!CopyFileW(from.c_str(), temporary.c_str(), FALSE) ||
                !MoveFileExW(temporary.c_str(), to.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
                error(); DeleteFileW(temporary.c_str()); ok = false; break;
            }
        }
    } while (FindNextFileW(find, &data));
    if (ok && GetLastError() != ERROR_NO_MORE_FILES) { error(); ok = false; }
    FindClose(find);
    return ok;
}
}
char *user_config_path()
{
    auto root = state_root();
    return root.empty() ? nullptr : utf8((root + L"\\amos.conf").c_str());
}
char *user_data_directory()
{
    auto root = state_root();
    return root.empty() ? nullptr : utf8(root.c_str());
}
bool refresh_user_resources(const char *resources, const char *target)
{
    auto from = wide(resources), to = wide(target);
    return !from.empty() && !to.empty() && seed(from, to);
}
char *prepare_installed_state(const char *resources)
{
    if (getenv("AMOS_HOME")) return _strdup(resources);
    char *exe = executable_path();
    if (!exe) return nullptr;
    std::string marker = exe; free(exe);
    marker.resize(marker.find_last_of('/') + 1);
    marker += "installed.ini";
    auto installed = wide(marker.c_str());
    if (GetFileAttributesW(installed.c_str()) == INVALID_FILE_ATTRIBUTES) return _strdup(resources);
    auto root = state_root();
    if (root.empty()) return nullptr;
    // Serialize seeding for independent document-opening processes of this user.
    uint64_t hash = 14695981039346656037ull;
    for (wchar_t c : root) { hash ^= uint16_t(c); hash *= 1099511628211ull; }
    std::wstring name = L"Local\\AMOSystem.Seed." + std::to_wstring(hash);
    HANDLE mutex = CreateMutexW(nullptr, FALSE, name.c_str());
    if (!mutex) { error(); return nullptr; }
    DWORD wait = WaitForSingleObject(mutex, 30000);
    if (wait != WAIT_OBJECT_0 && wait != WAIT_ABANDONED) { CloseHandle(mutex); errno = EBUSY; return nullptr; }
    auto runtime = root + L"\\runtime";
    char *target = utf8(runtime.c_str());
    bool ok = target && refresh_user_resources(resources, target);
    free(target);
    ReleaseMutex(mutex); CloseHandle(mutex);
    return ok ? utf8(runtime.c_str()) : nullptr;
}
char *canonical_path(const char *path)
{
    auto w = wide(path);
    if (w.empty()) return nullptr;
    HANDLE h = CreateFileW(w.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                          nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
    if (h == INVALID_HANDLE_VALUE) { error(); return nullptr; }
    DWORD n = GetFinalPathNameByHandleW(h, nullptr, 0, FILE_NAME_NORMALIZED | VOLUME_NAME_DOS);
    std::vector<wchar_t> buffer(size_t(n) + 1);
    DWORD got = n ? GetFinalPathNameByHandleW(h, buffer.data(), DWORD(buffer.size()),
                                            FILE_NAME_NORMALIZED | VOLUME_NAME_DOS) : 0;
    CloseHandle(h);
    if (!got || got >= buffer.size()) { errno = EIO; return nullptr; }
    std::wstring result = buffer.data();
    if (result.compare(0, 8, L"\\\\?\\UNC\\") == 0) result = L"\\\\" + result.substr(8);
    else if (result.compare(0, 4, L"\\\\?\\") == 0) result.erase(0, 4);
    return utf8(result.c_str());
}
char *executable_path()
{
    std::vector<wchar_t> buffer(32768);
    DWORD n = GetModuleFileNameW(nullptr, buffer.data(), DWORD(buffer.size()));
    if (!n || n >= buffer.size()) { errno = ENAMETOOLONG; return nullptr; }
    return utf8(buffer.data());
}
bool image_below_4gb()
{
    auto *base = reinterpret_cast<const unsigned char *>(GetModuleHandleW(nullptr));
    auto *dos = reinterpret_cast<const IMAGE_DOS_HEADER *>(base);
    auto *pe = reinterpret_cast<const IMAGE_NT_HEADERS64 *>(base + dos->e_lfanew);
    uintptr_t at = reinterpret_cast<uintptr_t>(base);
    return at < (uintptr_t(1) << 32) && pe->OptionalHeader.SizeOfImage <= (uintptr_t(1) << 32) - at;
}
int make_directory(const char *path)
{
    auto w = wide(path);
    if (w.empty()) return -1;
    if (CreateDirectoryW(w.c_str(), nullptr)) return 0;
    error(); return -1;
}
char *temporary_directory()
{
    wchar_t path[32768];
    DWORD n = GetTempPathW(32768, path);
    if (!n || n >= 32768) { error(); return nullptr; }
    for (unsigned i = 0; i < 1024; i++) {
        std::wstring name = std::wstring(path) + L"amos-ram-" + std::to_wstring(GetCurrentProcessId()) +
                            L"-" + std::to_wstring(GetTickCount64()) + L"-" + std::to_wstring(i);
        if (CreateDirectoryW(name.c_str(), nullptr)) return utf8(name.c_str());
        if (GetLastError() != ERROR_ALREADY_EXISTS) { error(); return nullptr; }
    }
    errno = EEXIST; return nullptr;
}
bool remove_tree(const char *path) { auto w = wide(path); return !w.empty() && remove_wide(w); }
bool disk_space(const char *path, uint64_t *total, uint64_t *available)
{
    auto w = wide(path);
    if (w.empty()) return false;
    DWORD attr = GetFileAttributesW(w.c_str());
    if (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY)) {
        auto slash = w.find_last_of(L"/\\");
        if (slash != std::wstring::npos) w.resize(slash + 1);
    }
    ULARGE_INTEGER avail, bytes, free;
    if (!GetDiskFreeSpaceExW(w.c_str(), &avail, &bytes, &free)) { error(); return false; }
    *total = bytes.QuadPart; *available = avail.QuadPart;
    return true;
}
bool file_details(const char *path, uint64_t *device, uint64_t *inode, int64_t *sec, int64_t *nsec)
{
    auto w = wide(path);
    if (w.empty()) return false;
    HANDLE h = CreateFileW(w.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                          nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, nullptr);
    if (h == INVALID_HANDLE_VALUE) { error(); return false; }
    BY_HANDLE_FILE_INFORMATION info;
    bool ok = GetFileInformationByHandle(h, &info) != 0;
    if (!ok) error();
    CloseHandle(h);
    if (!ok) return false;
    *device = info.dwVolumeSerialNumber;
    *inode = uint64_t(info.nFileIndexHigh) << 32 | info.nFileIndexLow;
    FILETIME t = info.ftCreationTime;
    if (!(t.dwHighDateTime | t.dwLowDateTime)) t = info.ftLastWriteTime;
    int64_t ticks = int64_t(uint64_t(t.dwHighDateTime) << 32 | t.dwLowDateTime) - 116444736000000000ll;
    *sec = ticks / 10000000; *nsec = ticks % 10000000 * 100;
    return true;
}
int environment_set(const char *name, const char *value, int overwrite)
{
    if (!overwrite && getenv(name)) return 0;
    return _putenv_s(name, value);
}
void *library_open(const char *name)
{
    // Optional third-party DLLs are application-local, not searched in cwd.
    char *exe = executable_path();
    if (!exe) return nullptr;
    std::string path = exe; free(exe);
    path.resize(path.find_last_of('/') + 1); path += name;
    auto w = wide(path.c_str());
    return w.empty() ? nullptr : reinterpret_cast<void *>(LoadLibraryExW(w.c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH));
}
void *library_symbol(void *library, const char *name)
{
    return reinterpret_cast<void *>(GetProcAddress(static_cast<HMODULE>(library), name));
}
void *fiber_create(size_t size, void (*entry)())
{
    auto *start = new FiberStart{entry};
    void *fiber = CreateFiberEx(size, size, FIBER_FLAG_FLOAT_SWITCH, fiber_entry, start);
    if (!fiber) delete start;
    return fiber;
}
void fiber_delete(void *fiber) { DeleteFiber(fiber); }
void *fiber_current()
{
    // Same GS-relative read as MinGW's GetCurrentFiber. A volatile offset
    // avoids GCC 13's false array-bounds warning for its constant-offset intrinsic.
    volatile DWORD offset = FIELD_OFFSET(NT_TIB, FiberData);
    return reinterpret_cast<void *>(__readgsqword(offset));
}
void fiber_switch(void *fiber) { SwitchToFiber(fiber); }
uint64_t monotonic_microseconds()
{
    LARGE_INTEGER frequency, counter;
    if (!QueryPerformanceFrequency(&frequency) || !QueryPerformanceCounter(&counter)) {
        fprintf(stderr, "amos: Windows monotonic timer unavailable\n");
        exit(70);
    }
    uint64_t ticks = uint64_t(counter.QuadPart), hz = uint64_t(frequency.QuadPart);
    return ticks / hz * 1000000 + ticks % hz * 1000000 / hz;
}
bool run_main(void (*entry)())
{
    void *caller = ConvertThreadToFiberEx(nullptr, FIBER_FLAG_FLOAT_SWITCH);
    if (!caller) return false;
    void *fiber = fiber_create(8 * 1024 * 1024, entry);
    if (!fiber) { ConvertFiberToThread(); return false; }
    SwitchToFiber(fiber);
    // AMOS exits through main_get_out; returning here is a startup error.
    DeleteFiber(fiber);
    ConvertFiberToThread();
    return false;
}
FILE *stream_open(const char *path, const char *mode)
{
    auto w = wide(path), m = wide(mode);
    return w.empty() || m.empty() ? nullptr : _wfopen(w.c_str(), m.c_str());
}
int path_open(const char *path, int flags, int mode)
{
    auto w = wide(path);
    return w.empty() ? -1 : _wopen(w.c_str(), flags | _O_BINARY, mode);
}
int path_stat(const char *path, struct stat *st)
{
    auto w = wide(path);
    struct _stat64 native;
    if (w.empty() || _wstat64(w.c_str(), &native)) return -1;
    memset(st, 0, sizeof *st);
    st->st_dev = native.st_dev; st->st_ino = native.st_ino;
    st->st_mode = native.st_mode; st->st_nlink = native.st_nlink;
    st->st_uid = native.st_uid; st->st_gid = native.st_gid;
    st->st_rdev = native.st_rdev; st->st_size = native.st_size;
    st->st_atime = native.st_atime; st->st_mtime = native.st_mtime; st->st_ctime = native.st_ctime;
    return 0;
}
int path_unlink(const char *path) { auto w = wide(path); return w.empty() ? -1 : _wunlink(w.c_str()); }
int path_rmdir(const char *path) { auto w = wide(path); return w.empty() ? -1 : _wrmdir(w.c_str()); }
int path_rename(const char *from, const char *to)
{
    auto f = wide(from), t = wide(to);
    return f.empty() || t.empty() ? -1 : _wrename(f.c_str(), t.c_str());
}
int path_chdir(const char *path) { auto w = wide(path); return w.empty() ? -1 : _wchdir(w.c_str()); }
char *path_getcwd(char *buffer, int size)
{
    wchar_t *cwd = _wgetcwd(nullptr, 0);
    if (!cwd) return nullptr;
    char *result = utf8(cwd); free(cwd);
    if (!result || !buffer) return result;
    if (size < 0 || strlen(result) >= size_t(size)) { free(result); errno = ERANGE; return nullptr; }
    strcpy(buffer, result); free(result); return buffer;
}
DIR *directory_open(const char *path)
{
    auto w = wide(path);
    return w.empty() ? nullptr : reinterpret_cast<DIR *>(_wopendir(w.c_str()));
}
struct dirent *directory_read(DIR *dir)
{
    static struct dirent entry;
    auto *found = _wreaddir(reinterpret_cast<_WDIR *>(dir));
    if (!found) return nullptr;
    char *name = utf8(found->d_name);
    if (!name) return nullptr;
    if (strlen(name) >= sizeof entry.d_name) { free(name); errno = ENAMETOOLONG; return nullptr; }
    memset(&entry, 0, sizeof entry);
    strcpy(entry.d_name, name); entry.d_namlen = strlen(name);
    free(name); return &entry;
}
int directory_close(DIR *dir) { return _wclosedir(reinterpret_cast<_WDIR *>(dir)); }

}
#endif
