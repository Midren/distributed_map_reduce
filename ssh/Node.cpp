// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Node.h"

Node::Node(const Node &node) {
    this->session.optionsCopy(node.session);
    this->is_connected = node.is_connected;
}

Node::Node(std::string node_name) : session(), is_connected(false) {
    session.setOption(SSH_OPTIONS_HOST, node_name.c_str());
};

Node::~Node() {
    if (is_connected) {
        session.disconnect();
    }
}

void Node::connect() {
    session.connect();
    is_connected = true;
    if (session.userauthPublickeyAuto() != SSH_AUTH_SUCCESS) {
        throw std::runtime_error("Didn't auth");
    }
}


std::string Node::execute_command(const std::string& cmd, bool is_output) {
    ssh::Channel channel(session);
    channel.openSession();
    ssh_channel_request_pty(channel.getCChannel());
    channel.requestExec(cmd.c_str());
    std::string ret;
    if (is_output) {
        int nbytes;
        char buffer[256];
        nbytes = channel.read(buffer, sizeof(buffer), false, -1);
        while (nbytes > 0) {
            ret.append(buffer);
            nbytes = ssh_channel_read(channel.getCChannel(), buffer, sizeof(buffer), false);
        }
    }
    return ret;
}

void Node::scp_write_file(std::filesystem::path path_to_file, const std::string& text) {
    Scp scp(session, SSH_SCP_WRITE, path_to_file.parent_path());
    scp.push_file(path_to_file.filename().c_str(), text.length(), S_IRUSR | S_IWUSR);
    scp.write(text);
}

std::string Node::scp_read_file(std::filesystem::path path_to_file) {
    Scp scp(session, SSH_SCP_READ, path_to_file.c_str());

    if (scp.pull_request() != SSH_SCP_REQUEST_NEWFILE) {
        throw std::runtime_error("Invalid request from scp");
    }

    scp.accept_request();
    scp.pull_request();
    std::string file = scp.read();

    return file;
}

void Node::scp_send_file(std::filesystem::path from, std::filesystem::path to) {
    std::ifstream input(from, std::ifstream::binary);
    if (input.is_open()) {
        std::string data = static_cast<std::ostringstream &>(std::ostringstream{} << input.rdbuf()).str();
        Node::scp_write_file(to, data);
    }
    input.close();
};

void Node::scp_download_file(std::filesystem::path from, std::filesystem::path to) {
    std::string input = scp_read_file(from);
    std::ofstream(to, std::ios::binary).write(input.c_str(), input.length());
}


