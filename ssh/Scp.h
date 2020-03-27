// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef CONCURRENT_MAKE_SCP_H
#define CONCURRENT_MAKE_SCP_H

#include "libssh/libsshpp.hpp"
#include <stdexcept>

class Scp {
public:
    Scp(ssh::Session &session, int mode, std::string location);

    ~Scp();

    void push_file(const std::string &filename, size_t text_size, int perms);

    void write(const std::string &text);

    void accept_request();

    int pull_request();

    std::string read();

private:
    ssh_scp scp;
};


#endif //CONCURRENT_MAKE_SCP_H
