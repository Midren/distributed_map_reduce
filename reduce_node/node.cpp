#include <boost/asio.hpp>

#include "server.h"

int main() {
    boost::asio::io_context io_service;

    server s(io_service, 8001);

    io_service.run();

//    //TODO: Run reduce on data
//
//    //TODO: Send result somewhere
//    std::cout << to_csv(key_value_outs);
}

