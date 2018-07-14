//
// Created by rmqi on 11/7/18.
//

#include "Socket.h"
#include <netinet/tcp.h>

void RN::Socket::bindAddress(const RN::InetAddress &addr) {
    sockets::bindOrDie(sockfd_, addr.getSockAddrInet());
}

void RN::Socket::listen() {
    sockets::listenOrDie(sockfd_);

}

int RN::Socket::accept(RN::InetAddress *peeraddr) {
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    int connfd = sockets::accept(sockfd_, &addr);
    if (connfd >= 0) {
        peeraddr->setSockAddrInet(addr);
    }
    return connfd;
}

void RN::Socket::setReuseAddr(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

}


void RN::Socket::setTcpNoDelay(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}


void RN::Socket::setKeepAlive(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}

void RN::Socket::shutdownWrite() {
    sockets::shutdownWrite(sockfd_);
}
