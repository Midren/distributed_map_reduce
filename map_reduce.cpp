// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "map_reduce.h"

#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/use_future.hpp>

#include "ssh/node.h"
#include "util.h"
#include "configurator/config.h"


namespace map_reduce {
    static std::future<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>>
    get_result(const std::shared_ptr<JobConfig> &cfg) {
        std::promise<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType >>> promise;
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

    static void
    send_config(ssh::node &n, const fs::path &base_directory, const fs::path &dll_path,
                const std::string &config_name) {
        n.execute_command("mkdir " + ("~" / base_directory).string() + " 2> /dev/null", false);
        n.scp_send_file(dll_path, base_directory / config_name);
        n.execute_command("chmod +x " + ("~" / base_directory / config_name).string(), false);
    }

    void
    run_reduce_node(const std::string &reduce_address, const std::string &master_address, const size_t map_num,
                    const fs::path &base_directory, const fs::path &dll_path) {
        auto[reduce_ip, reduce_port] = parse_ip_port(reduce_address);
        auto[master_ip, master_port] = parse_ip_port(master_address);
        const fs::path reduce_node_path("reduce_node");
        ssh::node reduce_node(reduce_ip);

        reduce_node.connect();
        send_config(reduce_node, base_directory, dll_path, "libreduce_config.so");
        reduce_node.execute_command(
                "cd " + ("~" / base_directory).string() +
                "&& export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:" + ("~" / base_directory).string() +
                "&& nohup " + reduce_node_path.string() +
                " --input_num=" + std::to_string(map_num) +
                " --master_node_address=" + master_ip + ":" + std::to_string(master_port) +
                " --port=" + std::to_string(reduce_port) +
                " --config_file=libreduce_config.so > reduce.out 2> reduce.err < /dev/null &",
                false);
    }

    void run_map_nodes(const std::vector<std::string> &map_ips, const std::string &reduce_address,
                       const fs::path &map_input_file, const fs::path &base_directory, const fs::path &dll_path) {
        auto[reduce_ip, reduce_port] = parse_ip_port(reduce_address);
        const fs::path map_node_path("map_node");
        for (auto &map_ip: map_ips) {
            ssh::node map_node(map_ip);
            map_node.connect();
            send_config(map_node, base_directory, dll_path, "libmap_config.so");
            map_node.execute_command(
                    "cd " + ("~" / base_directory).string() +
                    "&& export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:" + ("~" / base_directory).string() +
                    "&& " + map_node_path.string() +
                    " --input_file=" + map_input_file.string() +
                    " --reduce_node_address=" + reduce_ip + ":" + std::to_string(reduce_port) +
                    " --config_file=libmap_config.so > map.out 2> map.err < /dev/null",
                    false);
        }
    }

    std::future<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>>
    run_task(const std::vector<std::string> &map_ips, const std::string &reduce_address,
             const std::string &master_address,
             const fs::path &map_input_file, const fs::path &dll_path) {
        const fs::path base_directory = ".cache/mapreduce";

        auto library_handler = get_config_dll_handler(dll_path.filename());
        auto cfg = get_config(library_handler);
        auto future = get_result(cfg);

        try {
            ssh_init();
            run_reduce_node(reduce_address, master_address, map_ips.size(), base_directory, dll_path);
            run_map_nodes(map_ips, reduce_address, map_input_file, base_directory, dll_path);
            ssh_finalize();
        } catch (ssh::SshException &e) {
            std::cerr << e.getError() << std::endl;
        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
        return future;
    }


    std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>
    run_task_blocking(const std::vector<std::string> &map_ips, const std::string &reduce_address,
                      const std::string &master_address,
                      const fs::path &map_input_file, const fs::path &dll_path) {
        auto future = run_task(map_ips, reduce_address, master_address, map_input_file, dll_path);
        future.wait();
        return future.get();
    }
}
