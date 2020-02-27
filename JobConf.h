#ifndef MAP_REDUCE_JOBCONF_H
#define MAP_REDUCE_JOBCONF_H

#include "ReduceBase.h"
#include "MapBase.h"

class JobConf {
public:
    JobConf() = default;

    JobConf(const std::string &job_name, std::unique_ptr<MapBase> &&map_class,
            std::unique_ptr<ReduceBase> &&reduce_class)
            : job_name(job_name), map_class(std::move(map_class)), reduce_class(std::move(reduce_class)) {};

    void setJobName(const std::string &job_name) {
        this->job_name = job_name;
    }

    std::string getJobName() {
        return job_name;
    }

    void setMapClass(std::unique_ptr<MapBase> &&map) {
        this->map_class = std::move(map);
    }

    MapBase *getMapClass() {
        return map_class.get();
    }

    void setReduceClass(std::unique_ptr<MapBase> &&map) {
        this->map_class = std::move(map);
    }

    ReduceBase *getReduceClass() {
        return reduce_class.get();
    }


private:
    std::string job_name;
    std::unique_ptr<MapBase> map_class;
    std::unique_ptr<ReduceBase> reduce_class;
};

extern JobConf job_config;

extern "C"
{
typedef JobConf *(*get_config_t)();

JobConf *get_config();
}

#endif //MAP_REDUCE_JOBCONF_H
