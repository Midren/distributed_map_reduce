#include <iostream>

#include "../util.h"
#include "../types/KeyValueType.h"
#include "reduce.h"

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session> {
public:
    session(tcp::socket socket) : socket_(std::move(socket)) {}

    void start() {
        do_read();
    }

private:
    void do_read(std::string json = "") {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                [this, self = shared_from_this(), &json](boost::system::error_code ec,
                                                                         std::size_t length) {
                                    if (!ec) {
                                        do_read(std::move(json + std::string(data_, length)));
                                    } else {
                                        groupby(json);
                                        socket_.close();
                                    }
                                });
    }

    void do_write(std::size_t length) {
        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
                                 [this, self, length](boost::system::error_code ec, std::size_t /*length*/) {
                                     if (!ec)
                                         do_write(length);
                                 });
    }

    tcp::socket socket_;
    enum {
        max_length = 1024
    };
    char data_[max_length];
};

class server {
public:
    server(boost::asio::io_context &io_service, short port)
            : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
              socket_(io_service) {
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
            if (!ec)
                std::make_shared<session>(std::move(socket_))->start();
            do_accept();
        });
    }

    tcp::acceptor acceptor_;
    tcp::socket socket_;
};

int main() {
    try {
        boost::asio::io_context io_service;

        using namespace std; // For atoi.
        server s(io_service, 8001);

        io_service.run();
    }
    catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

//    //TODO: Group by key_out
//
//    //TODO: Run reduce on data
//
//    //TODO: Send result somewhere
//    std::cout << to_csv(key_value_outs);
}
