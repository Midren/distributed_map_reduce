// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <numeric>
#include <boost/algorithm/string.hpp>
#include <memory>
#include <algorithm>
#include "../../configurator/job_config.h"

using map_reduce::KeyValueType;
using map_reduce::IntKeyValueType;
using map_reduce::StringKeyValueType;
using map_reduce::IntKeyValueTypeFactory;
using map_reduce::StringKeyValueTypeFactory;

#include "boost/locale/boundary.hpp"
#include "boost/locale.hpp"

namespace ba=boost::locale::boundary;

std::vector<std::string> split_to_words(std::string &data) {
    boost::locale::generator gen;
    std::locale::global(gen("en_US.UTF-8"));

    std::vector<std::string> words_vec;
    ba::ssegment_index words(ba::word, data.begin(), data.end());
    for (auto &&x: words) {
        std::string word = boost::locale::fold_case(boost::locale::normalize(x.str()));
        if (word.length() >= 1 && std::any_of(word.begin(), word.end(), [](const auto &c) { return isalpha(c); }))
            words_vec.push_back(move(word));
    }
    return words_vec;
}

class word_count_map : public map_reduce::map_base {
public:
    void map(const std::unique_ptr<KeyValueType> &key, const std::unique_ptr<KeyValueType> &value) override {
        auto key_in = dynamic_cast<IntKeyValueType &>(*key).value;
        auto value_in = dynamic_cast<StringKeyValueType &>(*value).value;

        std::vector<std::string> words = split_to_words(value_in);
        for (auto &word: words) {
            if (word.empty())
                continue;
            emit(std::make_unique<StringKeyValueType>(word),
                 std::make_unique<IntKeyValueType>(1));
        }
    };

    std::unique_ptr<map_reduce::map_base> clone() override {
        return std::make_unique<word_count_map>();
    }
};

class sum_reduce : public map_reduce::reduce_base {
public:
    std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>
    reduce(const std::unique_ptr<KeyValueType> &key,
           const std::vector<std::unique_ptr<KeyValueType>> &outputs) override {

        const auto key_out = dynamic_cast<StringKeyValueType &>(*key).value;
        const auto value_res = std::accumulate(outputs.begin(), outputs.end(), 0, [](int lhs, const auto &rhs) {
            const auto value_out = dynamic_cast<IntKeyValueType &>(*rhs).value;
            return lhs + value_out;
        });

        return {std::make_unique<StringKeyValueType>(key_out),
                std::make_unique<IntKeyValueType>(value_res)};
    }
};

std::shared_ptr<job_config> get_config() {
    return std::make_shared<job_config>("Count words",
                                        std::move(std::make_unique<word_count_map>()),
                                        std::move(std::make_unique<sum_reduce>()),
                                        std::move(std::make_unique<IntKeyValueTypeFactory>()),
                                        std::move(std::make_unique<StringKeyValueTypeFactory>()),
                                        std::move(std::make_unique<StringKeyValueTypeFactory>()),
                                        std::move(std::make_unique<IntKeyValueTypeFactory>()),
                                        std::move(std::make_unique<IntKeyValueTypeFactory>())
    );
}
