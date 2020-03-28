// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>

#include <boost/asio.hpp>
#include <boost/asio/use_future.hpp>

#include "util.h"
#include "configurator/config.h"

std::future<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>>
get_result(const std::shared_ptr<JobConfig> &cfg) {
    std::promise<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>> promise;
    auto future = promise.get_future();

    std::thread([](std::promise<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>> promise,
                   const std::shared_ptr<JobConfig> &cfg) {
        using namespace boost::asio::ip;
        boost::asio::io_context io_service;
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 8002));
        tcp::socket socket(io_service);
        acceptor.async_accept(socket, [&socket, cfg, &promise](const boost::system::error_code &err) {
            std::array<char, 128> buf{};
            boost::system::error_code error;
            std::string json;
            for (;;) {
                size_t len = socket.read_some(boost::asio::buffer(buf), error);
                if (error == boost::asio::error::eof)
                    break; // Connection closed cleanly by peer.
                else if (error)
                    throw boost::system::system_error(error); // Some other error.
                json += std::string(buf.data(), len);
            }
            if (cfg->key_out_factory == nullptr ||
                cfg->value_res_factory == nullptr) {
                throw std::runtime_error("Key/value factories are null");
            }
            auto[key, value] = get_key_value_from_json(json, cfg->key_out_factory, cfg->value_res_factory);
            promise.set_value(std::make_pair(std::move(key), std::move(value)));
        });
        io_service.run();
    }, std::move(promise), std::cref(cfg)).detach();

    return future;
}

std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>
get_result_blocking(const std::shared_ptr<JobConfig> cfg) {
    auto future = get_result(cfg);
    future.wait();
    return future.get();
}

int main() {
    auto library_handler = get_config_dll_handler("libmap_reduce_config.so");
    auto cfg = get_config(library_handler);

    auto[key, value] = get_result_blocking(cfg);

    std::cout << "The result of Map/Reduce is " << value->to_string() << std::endl;

    return 0;
}
