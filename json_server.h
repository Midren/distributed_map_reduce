#ifndef MAP_REDUCE_REDUCE_SERVER_H
#define MAP_REDUCE_REDUCE_SERVER_H

#include <boost/asio.hpp>
#include <utility>

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, std::function<void(std::string)> json_handler) : socket_(std::move(socket)),
                                                                                 json_handler(
                                                                                         std::move(json_handler)) {}

    void start() {
        do_read();
    }

private:
    void do_read(const std::string &json = "") {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                [this, self = shared_from_this(), json](boost::system::error_code ec,
                                                                        std::size_t length) {
                                    if (!ec) {
                                        do_read(json + std::string(data_, length));
                                    } else {
                                        json_handler(json);
                                        socket_.close();
                                    }
                                });
    }

    tcp::socket socket_;
    enum {
        max_length = 1024
    };
    char data_[max_length];
    std::function<void(const std::string &)> json_handler;
};

class JsonServer {
public:
    JsonServer(boost::asio::io_context &io_service, short port, std::function<void(const std::string &)> handler)
            : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
              socket_(io_service), json_handler(std::move(handler)) {
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
            if (!ec)
                std::make_shared<Session>(std::move(socket_), json_handler)->start();
            do_accept();
        });
    }

    tcp::acceptor acceptor_;
    tcp::socket socket_;
    std::function<void(const std::string &)> json_handler;
};

#endif //MAP_REDUCE_REDUCE_SERVER_H
