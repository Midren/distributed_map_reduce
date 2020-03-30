// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef MAP_REDUCE_CONFIG_H
#define MAP_REDUCE_CONFIG_H

#include <filesystem>
#include <dlfcn.h>

#include "JobConfig.h"

std::shared_ptr<void> get_config_dll_handler(const std::filesystem::path &path);

std::shared_ptr<JobConfig> get_config(const std::shared_ptr<void> &config_dll_handler);

#endif //MAP_REDUCE_CONFIG_H
