#include <fstream>
#include <filesystem>

#include "../util.h"
#include "../configurator/JobConfig.h"
#include "../configurator/config.h"

#include <boost/asio.hpp>

std::string read_data(const std::filesystem::path &) {
    std::ifstream fin{"input.csv"};
    if (!fin.is_open()) {
        throw std::runtime_error("Cannot open input file");
    }
    return dynamic_cast<std::stringstream &>(std::stringstream{} << fin.rdbuf()).str();
}

int main() {
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"), 8001);
    boost::asio::io_service service;

    auto library_handler = get_config_dll_handler("libmap_reduce_config.so");
    auto cfg = get_config(library_handler);

    auto data = read_data("input.csv");

    auto key_value_ins = get_key_values_from_csv(data, cfg->key_in_factory, cfg->value_in_factory);

    std::for_each(key_value_ins.begin(), key_value_ins.end(),
                  [&cfg, &ep, &service](const auto &key_value) {
                      const auto &[key, value] = key_value;;
                      auto res = cfg->map_class->map(key, value);

                      boost::asio::ip::tcp::socket sock(service);
                      //TODO: checking if no server
                      sock.connect(ep);
                      sock.write_some(boost::asio::buffer(to_json(res)));
                      sock.close();
                  });
}
