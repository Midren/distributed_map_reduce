#include "Scp.h"

Scp::Scp(ssh::Session &session, int mode, std::string location) {
    scp = ssh_scp_new(session.getCSession(), mode, location.c_str());
    if (scp == nullptr) {
        throw std::runtime_error("Cannot create scp");
    }
    if (ssh_scp_init(scp) != SSH_OK) {
        ssh_scp_free(scp);
        throw std::runtime_error("Cannot initialize scp");
    }
}

Scp::~Scp() {
    ssh_scp_close(scp);
    ssh_scp_free(scp);
}

void Scp::push_file(std::string filename, size_t text_size, int perms) {
    if (ssh_scp_push_file(scp, filename.c_str(), text_size, perms) != SSH_OK) {
        throw std::runtime_error("Cannot open remote file");
    }
}

void Scp::write(std::string text) {
    if (ssh_scp_write(scp, text.c_str(), text.length()) != SSH_OK) {
        throw std::runtime_error("Cannot write to remote file");
    }
}

void Scp::accept_request() {
    ssh_scp_accept_request(scp);
}

int Scp::pull_request() {
    return ssh_scp_pull_request(scp);
}

std::string Scp::read() {
    size_t size = ssh_scp_request_get_size(scp);
    char *buffer = new char[size];
    if (ssh_scp_read(scp, buffer, size) == SSH_ERROR) {
        free(buffer);
        throw std::runtime_error("Error receiving file data");
    }
    std::string ret(buffer, size);
    free(buffer);
    return ret;
}
