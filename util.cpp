// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "util.h"

#include <iostream>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace map_reduce {
    std::pair<std::string, unsigned int> parse_ip_port(const std::string &address) {
        std::vector<std::string> ip_port;
        boost::split(ip_port, address, boost::is_any_of(":"));
        if (ip_port.size() != 2) {
            throw std::runtime_error("Address should have format ip:port");
        }
        return {ip_port[0], std::stoi(ip_port[1])};
    }

    std::vector<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>>
    get_key_values_from_csv(const std::string &data, std::unique_ptr<KeyValueTypeFactory> &key_factory,
                            std::unique_ptr<KeyValueTypeFactory> &value_factory, char delimiter, char end_of_line) {
        std::vector<std::string> key_values;
        boost::split(key_values, data, [end_of_line](auto x) {
            return x == end_of_line;
        });
        //Remove blank lines or ill-formed lines
        key_values.erase(std::remove_if(key_values.begin(), key_values.end(), [delimiter](const std::string &val) {
            return val.find(delimiter) == std::string::npos;
        }), key_values.end());

        std::vector<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>> key_value_ins;
        std::transform(key_values.begin() + 1, key_values.end(), std::back_inserter(key_value_ins),
                       [&key_factory, &value_factory, delimiter](const std::string &x) {
                           std::vector<std::string> key_value;
                           boost::split(key_value, x, [delimiter](auto x) {
                               return x == delimiter;
                           });
                           return make_pair(key_factory->create(key_value[0]),
                                            value_factory->create(key_value[1]));
                       });
        return key_value_ins;
    }

    std::string to_json(const std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>> &key_value) {
        boost::property_tree::ptree pt{};
        const auto &[key, value] = key_value;
        pt.put("key", key->to_string());
        pt.put("value", value->to_string());

        std::stringstream ss{};
        boost::property_tree::json_parser::write_json(ss, pt);
        return ss.str();
    }

    std::string data_end_message() {
        boost::property_tree::ptree pt{};
        pt.put(data_end_flag, true);

        std::stringstream ss{};
        boost::property_tree::json_parser::write_json(ss, pt);
        return ss.str();
    }


    void send_end_message(const boost::asio::ip::tcp::endpoint &reduce_ep) {
        boost::asio::io_service service;
        boost::system::error_code ec;
        boost::asio::ip::tcp::socket sock(service);
        sock.connect(reduce_ep, ec);
        sock.non_blocking(false);
        if (ec)
            throw std::runtime_error("cannot connect to node");
        sock.wait(sock.wait_write);
        boost::asio::write(sock, boost::asio::buffer(data_end_message()),
                           boost::asio::transfer_all(),
                           ec);
        if (ec)
            throw std::runtime_error("fail during writing to socket");
    }


    std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>
	get_key_value_from_json(const std::string &data, std::unique_ptr<KeyValueTypeFactory> &key_factory,
		std::unique_ptr<KeyValueTypeFactory> &value_factory) {
    boost::property_tree::ptree pt{};

    std::stringstream ss(data);

    boost::property_tree::json_parser::read_json(ss, pt);

    if (pt.get(data_end_flag, false))
        throw data_ended_error();

    return {key_factory->create(pt.get("key", "")),
            value_factory->create(pt.get("value", ""))};
}



    std::string
    to_csv(const std::vector<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>> &key_values,
           char delimeter, char end_of_line
    ) {
        std::stringstream data;
        data << "key" << delimeter << "value" << end_of_line;
        for (auto&[key, value]: key_values) {
            data << key->to_string() << delimeter << value->to_string() << end_of_line;
        }
        return data.str();
    }
}
