// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>
#include "node.h"

namespace ssh {
    node::node(const node &node) : session(ssh::Session{}), is_connected(false) {
        this->session.optionsCopy(node.session);
    }

    node::node(const std::string &node_name, const std::string &user) : session(ssh::Session{}),
                                                                        is_connected(false) {
        int verbosity = SSH_LOG_NONE;
        session.setOption(SSH_OPTIONS_HOST, node_name.c_str());
        session.setOption(SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
        session.setOption(SSH_OPTIONS_USER, user.c_str());
    };

    node::~node() {
        if (is_connected) {
            session.disconnect();
        }
    }

    void node::connect() {
        session.connect();
        is_connected = true;
        if (session.isServerKnown() != SSH_SERVER_KNOWN_OK) {
            if (session.writeKnownhost() != SSH_OK) {
                throw std::runtime_error("writeKnownHost failed");
            }
        }
        if (session.userauthPublickeyAuto() != SSH_AUTH_SUCCESS) {
            if (session.userauthPassword("mapreduce") != SSH_AUTH_SUCCESS) {
                throw std::runtime_error("Cannot auth to remote node");
            }
        }
    }


    std::string node::execute_command(const std::string &cmd, bool is_output) {
        ssh::Channel channel(session);
        channel.openSession();
        channel.requestExec(cmd.c_str());
        std::string ret;
        if (is_output) {
            std::array<char, 1024> buffer{};
            int cnt = channel.read(buffer.data(), buffer.max_size(), true, -1);
            while (cnt > 0) {
                ret += std::string(buffer.data(), cnt);
                cnt = channel.read(buffer.data(), buffer.max_size(), true, -1);
            }
        }
        channel.sendEof();
        channel.close();
        return ret;
    }

    void node::scp_write_file(const std::filesystem::path &path_to_file, const std::string &text) {
        scp scp(session, SSH_SCP_WRITE, path_to_file.parent_path());
        scp.push_file(path_to_file.filename().c_str(), text.length(), S_IRUSR | S_IWUSR);
        try {
            scp.write(text);
        } catch (std::exception &e) {
            throw std::runtime_error("Cannot write to remote file: " + path_to_file.string());
        }
    }

    std::string node::scp_read_file(const std::filesystem::path &path_to_file) {
        scp scp(session, SSH_SCP_READ, path_to_file.c_str());

        if (scp.pull_request() != SSH_SCP_REQUEST_NEWFILE) {
            throw std::runtime_error("Invalid request from scp");
        }

        scp.accept_request();
        scp.pull_request();

        return scp.read();
    }

    void node::scp_send_file(const std::filesystem::path &from, const std::filesystem::path &to) {
        std::ifstream input(from, std::ifstream::binary);
        if (input.is_open()) {
            std::string data = dynamic_cast<std::ostringstream &>(std::ostringstream{} << input.rdbuf()).str();
            input.close();
            scp_write_file(to, data);
        } else {
            throw std::runtime_error("Cannot find a file");
        }
    };

    void node::scp_download_file(const std::filesystem::path &from, const std::filesystem::path &to) {
        std::string input = scp_read_file(from);
        std::ofstream(to, std::ios::binary).write(input.c_str(), input.length());
    }

}
