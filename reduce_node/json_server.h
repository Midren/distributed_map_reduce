// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef MAP_REDUCE_REDUCE_SERVER_H
#define MAP_REDUCE_REDUCE_SERVER_H

#include <boost/asio.hpp>
#include <utility>

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, const std::function<void(const std::string &)> &json_handler) : socket_(
            std::move(socket)), json_handler(json_handler) {}

    void start() {
        do_read();
    }

private:
    void do_read(const std::string &json = "") {
        socket_.async_read_some(boost::asio::buffer(buf),
                                [this, self = shared_from_this(), json](boost::system::error_code ec,
                                                                        std::size_t length) {
                                    if (!ec) {
                                        do_read(json + std::string(buf.data(), length));
                                    } else {
                                        json_handler(json);
                                        socket_.close();
                                    }
                                });
    }

    tcp::socket socket_;
    static constexpr unsigned int max_length = 1024;
    std::array<char, max_length> buf;
    const std::function<void(const std::string &)> &json_handler;
};

class JsonServer {
public:
    JsonServer(boost::asio::io_context &io_service, short port, const std::function<void(const std::string &)> &handler)
            : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
              socket_(io_service), json_handler(handler) {
        do_accept();
    }

    void close() {
        acceptor_.close();
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
    const std::function<void(const std::string &)> &json_handler;
};

#endif //MAP_REDUCE_REDUCE_SERVER_H
