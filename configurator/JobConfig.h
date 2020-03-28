// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef MAP_REDUCE_JOBCONFIG_H
#define MAP_REDUCE_JOBCONFIG_H

#include <utility>
#include <memory>

#include "ReduceBase.h"
#include "MapBase.h"
#include "../types/KeyValueTypeFactory.h"

class JobConfig {
public:
    JobConfig(std::string job_name,
              std::unique_ptr<MapBase> map_class,
              std::unique_ptr<ReduceBase> reduce_class,
              std::unique_ptr<KeyValueTypeFactory> key_in_factory,
              std::unique_ptr<KeyValueTypeFactory> key_out_factory,
              std::unique_ptr<KeyValueTypeFactory> value_in_factory,
              std::unique_ptr<KeyValueTypeFactory> value_out_factory,
              std::unique_ptr<KeyValueTypeFactory> value_res_factory
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
    std::unique_ptr<MapBase> map_class;
    std::unique_ptr<ReduceBase> reduce_class;
    std::unique_ptr<KeyValueTypeFactory> key_in_factory;
    std::unique_ptr<KeyValueTypeFactory> key_out_factory;
    std::unique_ptr<KeyValueTypeFactory> value_in_factory;
    std::unique_ptr<KeyValueTypeFactory> value_out_factory;
    std::unique_ptr<KeyValueTypeFactory> value_res_factory;
};

extern JobConfig job_config;

extern "C"
{
typedef std::shared_ptr<JobConfig> (*get_config_t)();

std::shared_ptr<JobConfig> get_config();
}

#endif //MAP_REDUCE_JOBCONFIG_H
