//
// Created by rmqi on 11/7/18.
//

#include <RN/base/Logging.h>
#include <fcntl.h>
#include "SocketsOps.h"

using namespace RN;
namespace {
    typedef struct sockaddr SA;

    const SA *sockaddr_cast(const struct sockaddr_in *addr) {
        return static_cast<const SA *>(
                implicit_cast<const void *>(addr));
    }

    SA *sockaddr_cast(struct sockaddr_in *addr) {
        return static_cast< SA *>(
                implicit_cast<void *>(addr));
    }

#pragma GCC diagnostic ignored "-Wunused-function"

    void setNonBlockingAndCloseOnExec(int sockfd) {
        int flags = ::fcntl(sockfd, F_GETFL, 0);
        flags |= O_NONBLOCK;
        flags |= O_CLOEXEC;
        int ret = ::fcntl(sockfd, F_SETFL, flags);
        (void) ret;
    }


}

int RN::sockets::createNonblockingOrDie() {
    int sockfd = ::socket(
            AF_INET,
            SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
            IPPROTO_TCP);
    if (sockfd < 0) {
        LOG_SYSFATAL << "sockets::createNonblockingOrDie";
    }
    return sockfd;
}

void RN::sockets::bindOrDie(int sockfd, const struct sockaddr_in &addr) {
    int ret = ::bind(sockfd, sockaddr_cast(&addr), sizeof(addr));
    if (ret < 0) {
        LOG_SYSFATAL << "sockets::bindOrDie";
    }
}

int RN::sockets::accept(int sockfd, struct sockaddr_in *addr) {
    socklen_t addrlen = sizeof *addr;

    int connfd = ::accept4(sockfd,
                           sockaddr_cast(addr),
                           &addrlen,
                           SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd < 0) {
        int savedErrno = errno;
        LOG_SYSFATAL << "sockets::accept";
        switch (savedErrno) {
            case EAGAIN:
            case ECONNABORTED:
            case EINTR:
            case EPROTO: // ???
            case EPERM:
            case EMFILE: // per-process lmit of open file desctiptor ???
                // expected errors
                errno = savedErrno;
                break;
            case EBADF:
            case EFAULT:
            case EINVAL:
            case ENFILE:
            case ENOBUFS:
            case ENOMEM:
            case ENOTSOCK:
            case EOPNOTSUPP:
                // unexpected errors
                LOG_FATAL << "unexpected error of ::accept " << savedErrno;
                break;
            default:
                LOG_FATAL << "unknown error of ::accept " << savedErrno;
                break;

        }

    }
    return connfd;
}

void RN::sockets::close(int sockfd) {
    if (::close(sockfd) < 0) {
        LOG_SYSERR << "sockets::close";
    }
}

void RN::sockets::toHostPort(char *buf, size_t size, const struct sockaddr_in &addr) {
    char host[INET_ADDRSTRLEN] = "INVALID";
    ::inet_ntop(AF_INET, &addr.sin_addr, host, sizeof(host));
    uint16_t port = sockets::networkToHost16(addr.sin_port);
    snprintf(buf, size, "%s:%u", host, port);
}

void RN::sockets::fromHostPort(const char *ip, uint16_t port, struct sockaddr_in *addr) {
    addr->sin_family = AF_INET;
    addr->sin_port = hostToNetwork16(port);
    if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0) {
        LOG_SYSERR << "sockets::fromHostPort";
    }

}

void sockets::listenOrDie(int sockfd) {
    int ret = ::listen(sockfd, SOMAXCONN);
    if (ret < 0) {
        LOG_SYSFATAL << "sockets::listen";
    }
}

int sockets::getSocketError(int sockfd) {
    int optval;
    socklen_t optlen = sizeof optval;

    if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
        return errno;
    } else {
        return optval;
    }
}

void sockets::shutdownWrite(int sockfd) {
    if (::shutdown(sockfd, SHUT_WR) < 0) {
        LOG_SYSERR << "sockets::shutdownWrite";
    }

}
