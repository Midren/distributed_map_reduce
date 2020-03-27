// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>

#include <boost/asio.hpp>
#include <boost/asio/use_future.hpp>

#include "util.h"
#include "configurator/config.h"

std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>> blocking_get_result() {
    using namespace boost::asio::ip;
    boost::asio::io_context io_service;
    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 8002));
    tcp::socket socket(io_service);

    acceptor.accept(socket);
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
    auto library_handler = get_config_dll_handler("libmap_reduce_config.so");
    auto cfg = get_config(library_handler);
//    auto pair = get_key_value_from_json(json, cfg->key_out_factory, cfg->value_res_factory);
//    std::cout << "The result of Map/Reduce is " << value->to_string() << std::endl;
//    return {std::move(key), std::move(value)};
    auto pair = get_key_value_from_json(json, cfg->key_out_factory, cfg->value_res_factory);
    std::cout << "The result of Map/Reduce is " << pair.second->to_string() << std::endl;
    return pair;
}

int main() {

    auto[key, value] = blocking_get_result();
//    auto pair = blocking_get_result();

    std::cout << (value.get() == nullptr) << std::endl;
//    auto int_value = static_cast<IntKeyValueType *>(value.get());
    std::cout << "The result of Map/Reduce is " << value->to_string() << std::endl;

    return 0;
}
