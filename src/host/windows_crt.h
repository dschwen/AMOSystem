// UTF-8 paths at the host boundary; native CRT structures keep SDK packing.
#pragma once
#if defined(_WIN32)
#include <stddef.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
namespace amos::host::windows {
FILE *stream_open(const char *path, const char *mode);
int path_open(const char *path, int flags, int mode = 0);
int path_stat(const char *path, struct stat *st);
int path_unlink(const char *path);
int path_rmdir(const char *path);
int path_rename(const char *from, const char *to);
int path_chdir(const char *path);
char *path_getcwd(char *buffer, int size);
DIR *directory_open(const char *path);
struct dirent *directory_read(DIR *dir);
int directory_close(DIR *dir);
}
#ifndef AMOS_WINDOWS_CRT_IMPL
#define fopen(...) amos::host::windows::stream_open(__VA_ARGS__)
#define stat(...) amos::host::windows::path_stat(__VA_ARGS__)
#define unlink(...) amos::host::windows::path_unlink(__VA_ARGS__)
#define rmdir(...) amos::host::windows::path_rmdir(__VA_ARGS__)
#define chdir(...) amos::host::windows::path_chdir(__VA_ARGS__)
#define getcwd(...) amos::host::windows::path_getcwd(__VA_ARGS__)
#define opendir(...) amos::host::windows::directory_open(__VA_ARGS__)
#define readdir(...) amos::host::windows::directory_read(__VA_ARGS__)
#define closedir(...) amos::host::windows::directory_close(__VA_ARGS__)
#endif
#endif
