#include "config.h"

std::shared_ptr<void> get_config_dll_handler(const std::filesystem::path &path) {
    void *library_handler = dlopen(path.c_str(), RTLD_LAZY | RTLD_NODELETE);
    if (library_handler == nullptr) {
        throw std::runtime_error(dlerror());
    }
    return std::shared_ptr<void>(library_handler, [](void *handle) {
        dlclose(handle);
    });
}

std::shared_ptr<JobConfig> get_config(const std::shared_ptr<void> &config_dll_handler) {
    return ((get_config_t) dlsym(config_dll_handler.get(), "get_config"))();
}