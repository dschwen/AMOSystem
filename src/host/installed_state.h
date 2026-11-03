#pragma once
namespace amos::host {
// Native strings only; caller frees returned path with free().
char *prepare_macos_state(const char *resources);
bool refresh_posix_resources(const char *resources, const char *destination);
}
