// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef MAP_REDUCE_REDUCE_SERVER_H
#define MAP_REDUCE_REDUCE_SERVER_H

#include <iostream>
#include <boost/asio.hpp>
#include <utility>

namespace map_reduce {

    using boost::asio::ip::tcp;

    class session : public std::enable_shared_from_this<session> {
    public:
        session(tcp::socket socket, const std::shared_ptr<std::function<void(const std::string &)>> &json_handler)
                : socket_(
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
                                            json_handler->operator()(json);
                                            socket_.shutdown(tcp::socket::shutdown_send, ec);
                                        }
                                    });
        }

        tcp::socket socket_;
        static constexpr unsigned int max_length = 1024;
        std::array<char, max_length> buf;
        const std::shared_ptr<std::function<void(const std::string &)>> &json_handler;
    };

    class json_server {
    public:
        json_server(boost::asio::io_context &io_service, short port,
                    const std::shared_ptr<std::function<void(const std::string &)>> &handler)
                : acceptor_(io_service), socket_(io_service), json_handler(handler) {
            boost::system::error_code ec;
            auto endpoint = tcp::endpoint(tcp::v4(), port);

            acceptor_.open(endpoint.protocol(), ec);
            acceptor_.bind(endpoint, ec);
            acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);

            if (ec)
                throw std::runtime_error("Server creation error: " + ec.message());
            do_accept();
        }

    private:
        void do_accept() {
            acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
                if (!ec) {
                    std::make_shared<session>(std::move(socket_), json_handler)->start();
                } else {
                    std::cout << "Couldn't accept connection: " << ec.message() << std::endl;
                }
                do_accept();
            });
        }

        tcp::acceptor acceptor_;
        tcp::socket socket_;
        const std::shared_ptr<std::function<void(const std::string &)>> &json_handler;
    };
}

#endif //MAP_REDUCE_REDUCE_SERVER_H
