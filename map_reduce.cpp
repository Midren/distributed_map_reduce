// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "map_reduce.h"

#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/use_future.hpp>

#include "ssh/node.h"
#include "util.h"
#include "configurator/config.h"
#include "reduce_node/json_server.h"


namespace map_reduce {
    static std::future<std::vector<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>>>
    get_result(const std::shared_ptr<job_config> &cfg) {
        std::promise<std::vector<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType >>>> promise;
        auto future = promise.get_future();

        std::thread([cfg](
                std::promise<std::vector<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType >>>> promise) {
            boost::asio::io_context io_service;
            std::vector<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>> res;

            auto json_handler = std::make_shared<std::function<void(const std::string &)>>(
                    [&](const std::string &json) mutable {
                        try {
                            auto[key, value] = get_key_value_from_json(json, cfg->key_out_factory,
                                                                       cfg->value_res_factory);
                            res.emplace_back(std::move(key), std::move(value));
                        } catch (data_ended_error &e) {
                            promise.set_value(std::move(res));
                        }
                    });
            json_server s(io_service, 8002, json_handler);
            io_service.run();

        }, std::move(promise)).detach();

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

    std::future<std::vector<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>>>
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


    std::vector<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>>
    run_task_blocking(const std::vector<std::string> &map_ips, const std::string &reduce_address,
                      const std::string &master_address,
                      const fs::path &map_input_file, const fs::path &dll_path) {
        auto future = run_task(map_ips, reduce_address, master_address, map_input_file, dll_path);
        future.wait();
        return future.get();
    }
}
