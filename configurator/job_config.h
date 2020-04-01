// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef MAP_REDUCE_JOB_CONFIG_H
#define MAP_REDUCE_JOB_CONFIG_H

#include <utility>
#include <memory>

#include "reduce_base.h"
#include "map_base.h"
#include "../types/KeyValueTypeFactory.h"

class job_config {
public:
    job_config(std::string job_name,
               std::unique_ptr<map_reduce::map_base> map_class,
               std::unique_ptr<map_reduce::reduce_base> reduce_class,
               std::unique_ptr<map_reduce::KeyValueTypeFactory> key_in_factory,
               std::unique_ptr<map_reduce::KeyValueTypeFactory> key_out_factory,
               std::unique_ptr<map_reduce::KeyValueTypeFactory> value_in_factory,
               std::unique_ptr<map_reduce::KeyValueTypeFactory> value_out_factory,
               std::unique_ptr<map_reduce::KeyValueTypeFactory> value_res_factory
    )
            : job_name(std::move(job_name)),
              map_class(std::move(map_class)),
              reduce_class(std::move(reduce_class)),
              key_in_factory(std::move(key_in_factory)),
              key_out_factory(std::move(key_out_factory)),
              value_in_factory(std::move(value_in_factory)),
              value_out_factory(std::move(value_out_factory)),
              value_res_factory(std::move(value_res_factory)) {};

    std::string job_name;
    std::unique_ptr<map_reduce::map_base> map_class;
    std::unique_ptr<map_reduce::reduce_base> reduce_class;
    std::unique_ptr<map_reduce::KeyValueTypeFactory> key_in_factory;
    std::unique_ptr<map_reduce::KeyValueTypeFactory> key_out_factory;
    std::unique_ptr<map_reduce::KeyValueTypeFactory> value_in_factory;
    std::unique_ptr<map_reduce::KeyValueTypeFactory> value_out_factory;
    std::unique_ptr<map_reduce::KeyValueTypeFactory> value_res_factory;
};

extern "C"
{
typedef std::shared_ptr<job_config> (*get_config_t)();

std::shared_ptr<job_config> get_config();
}
#endif //MAP_REDUCE_JOB_CONFIG_H
