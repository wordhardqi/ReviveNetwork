//
// Created by rmqi on 11/7/18.
//

#ifndef RN_SOCKETS_H
#define RN_SOCKETS_H

#include "InetAddress.h"

namespace RN {

    class Socket {
    public:
        explicit Socket(int sockefd)
                : sockfd_(sockefd) {

        }

        ~Socket() {}

        int fd() const {
            return sockfd_;
        }

        void bindAddress(const InetAddress &addr);

        void listen();

        int accept(InetAddress *peeraddr);

        void setReuseAddr(bool on);

    private:
        const int sockfd_;
    };

}
#endif //RN_SOCKETS_H
