//
// Created by rmqi on 11/7/18.
//

#ifndef RN_INETADDRESS_H
#define RN_INETADDRESS_H

#include <RN/base/Types.h>
#include <RN/base/copyable.h>
#include <netinet/in.h>
#include <strings.h>
#include "SocketsOps.h"

namespace RN {
    static const in_addr_t kInaddrAny = INADDR_ANY;

    class InetAddress : RN::copyable {
    public:
        explicit InetAddress(uint16_t port) {
            bzero(&addr_, sizeof addr_);
            addr_.sin_family = AF_INET;
            addr_.sin_addr.s_addr = sockets::hostToNetwork32(kInaddrAny);
            addr_.sin_port = sockets::hostToNetwork16(port);
        }

        InetAddress(const string &ip, uint16_t port) {
            bzero(&addr_, sizeof addr_);
            sockets::fromHostPort(ip.c_str(), port, &addr_);

        }

        InetAddress(const struct sockaddr_in &addr)
                : addr_(addr) {
        }

        string toHostPort() const {
            char buf[32];
            sockets::toHostPort(buf, sizeof(buf), addr_);
            return buf;
        }

        void setSockAddrInet(const struct sockaddr_in &addr) {
            addr_ = addr;
        }

        const struct sockaddr_in &getSockAddrInet() const {
            return addr_;
        }


    private:
        struct sockaddr_in addr_;

    };
}

#endif //RN_INETADDRESS_H
