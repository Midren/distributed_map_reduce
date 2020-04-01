// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef CONCURRENT_MAKE_NODE_H
#define CONCURRENT_MAKE_NODE_H

#include <fcntl.h>
#include "libssh/libsshpp.hpp"
#include <fstream>
#include <filesystem>

#include "scp.h"

namespace ssh {

    class node {
    public:
        explicit node(const std::string &node_name, const std::string &user = "mapreduce");

        node(const node &node);

        ~node();

        void connect();

        std::string execute_command(const std::string &cmd, bool is_output);

        void scp_write_file(const std::filesystem::path &path_to_file, const std::string &text);

        std::string scp_read_file(const std::filesystem::path &path_to_file);

        void scp_send_file(const std::filesystem::path &from, const std::filesystem::path &to);

        void scp_download_file(const std::filesystem::path &from, const std::filesystem::path &to);


    private:
        ssh::Session session;
        bool is_connected;
    };

}
#endif //CONCURRENT_MAKE_NODE_H
