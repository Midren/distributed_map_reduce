#include <iostream>
#include <boost/asio/io_context.hpp>

#include "json_server.h"
#include "util.h"
#include "configurator/config.h"

int main() {

    boost::asio::io_context io_service;

    JsonServer s(io_service, 8002, [&io_service](std::string json) {
        auto library_handler = get_config_dll_handler("libmap_reduce_config.so");
        auto cfg = get_config(library_handler);
        auto[key, value] = get_key_value_from_json(json, cfg->key_out_factory, cfg->value_res_factory);
        std::cout << "The result of Map/Reduce is " << value->to_string() << std::endl;
        io_service.stop();
    });

    io_service.run();

    return 0;
}
