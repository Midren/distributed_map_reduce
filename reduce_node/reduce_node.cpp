#include <boost/asio.hpp>

#include "../json_server.h"
#include "reduce.h"

int main() {
    boost::asio::io_context io_service;

    JsonServer s(io_service, 8001, process);

    io_service.run();

//    //TODO: Run reduce on data
//
//    //TODO: Send result somewhere
//    std::cout << to_csv(key_value_outs);
}

