// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef MAP_REDUCE_CONFIG_H
#define MAP_REDUCE_CONFIG_H

#include <filesystem>
#include <dlfcn.h>

#include "JobConfig.h"

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

#endif //MAP_REDUCE_CONFIG_H
