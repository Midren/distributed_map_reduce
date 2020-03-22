#ifndef MAP_REDUCE_UTIL_H
#define MAP_REDUCE_UTIL_H

#include <sstream>

#include <boost/algorithm/string.hpp>

#include "types/KeyValueType.h"
#include "types/KeyValueTypeFactory.h"

std::vector<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>>
get_key_values_from_csv(const std::string &data, std::unique_ptr<KeyValueTypeFactory> &key_factory,
                        std::unique_ptr<KeyValueTypeFactory> &value_factory, char delimiter = ',',
                        char end_of_line = '\n') {
    std::vector<std::string> key_values;
    boost::split(key_values, data, [end_of_line](auto x) {
        return x == end_of_line;
    });

    std::vector<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>> key_value_ins;
    std::transform(key_values.begin() + 1, key_values.end(), std::back_inserter(key_value_ins),
                   [&key_factory, &value_factory, delimiter](const std::string &x) {
                       std::vector<std::string> key_value;
                       boost::split(key_value, x, [delimiter](auto x) {
                           return x == delimiter;
                       });
                       return make_pair(key_factory->create(key_value[0]),
                                        value_factory->create(key_value[1]));
                   });
    return key_value_ins;
}

std::string
to_csv(const std::vector<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>> &key_values,
       char delimeter = ',', char end_of_line = '\n') {
    std::stringstream data;
    data << "key" << delimeter << "value" << end_of_line;
    for (auto&[key, value]: key_values) {
        data << key->to_string() << delimeter << value->to_string() << end_of_line;
    }
    return data.str();
}


#endif //MAP_REDUCE_UTIL_H
