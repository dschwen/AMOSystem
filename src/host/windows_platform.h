// Native Windows services. Only primitive values cross the packed AMOS ABI.
#pragma once
#include <stddef.h>
#include <stdint.h>

namespace amos::host::windows {
char *canonical_path(const char *path); // malloc'ed; exists; '/' separators
char *prepare_installed_state(const char *resources); // malloc; portable mode leaves path unchanged
bool refresh_user_resources(const char *resources, const char *target);
char *user_data_directory(); // malloc; Known Folder based
char *user_config_path(); // malloc; Known Folder based
char *executable_path();               // malloc'ed
bool image_below_4gb();
int make_directory(const char *path);
char *temporary_directory();           // malloc'ed, creates unique AMOS RAM directory
bool remove_tree(const char *path);
bool disk_space(const char *path, uint64_t *total, uint64_t *available);
bool file_details(const char *path, uint64_t *device, uint64_t *inode, int64_t *sec, int64_t *nsec);
int environment_set(const char *name, const char *value, int overwrite);
void *library_open(const char *name);
void *library_symbol(void *library, const char *name);
void *fiber_create(size_t stack_size, void (*entry)());
void fiber_delete(void *fiber);
void *fiber_current();
void fiber_switch(void *fiber);
bool run_main(void (*entry)());
uint64_t monotonic_microseconds();
}
