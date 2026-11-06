#pragma once
namespace amos::host {
// Native strings only; caller frees returned path with free().
char *prepare_macos_state(const char *resources);
// Linux: the package's tree is read-only; a copy in $XDG_DATA_HOME/amos/runtime (nullptr on failure)
char *prepare_linux_state(const char *resources);
bool refresh_posix_resources(const char *resources, const char *destination);
}
