// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <memory>
#include "scp.h"

namespace ssh {
    scp::scp(ssh::Session &session, int mode, std::string location) {
        scp_ = ssh_scp_new(session.getCSession(), mode, location.c_str());
        if (scp_ == nullptr)
            throw std::runtime_error("Cannot create scp");

        if (ssh_scp_init(scp_) != SSH_OK) {
            ssh_scp_free(scp_);
            throw std::runtime_error("Cannot initialize scp");
        }
    }

    scp::~scp() {
        ssh_scp_close(scp_);
        ssh_scp_free(scp_);
    }

    void scp::push_file(const std::string &filename, size_t text_size, int perms) {
        if (ssh_scp_push_file(scp_, filename.c_str(), text_size, perms) != SSH_OK) {
            throw std::runtime_error("Cannot create remote file: " + filename);
        }
    }

    void scp::write(const std::string &text) {
        if (ssh_scp_write(scp_, text.c_str(), text.length()) != SSH_OK) {
            throw std::runtime_error("Cannot write to remote file");
        }
    }

    void scp::accept_request() {
        ssh_scp_accept_request(scp_);
    }

    int scp::pull_request() {
        return ssh_scp_pull_request(scp_);
    }

    std::string scp::read() {
        size_t size = ssh_scp_request_get_size(scp_);
        auto buffer = std::unique_ptr<char[]>(new char[size]);

        if (ssh_scp_read(scp_, buffer.get(), size) == SSH_ERROR)
            throw std::runtime_error("Error receiving file data");

        return std::string(buffer.get(), size);
    }
}
