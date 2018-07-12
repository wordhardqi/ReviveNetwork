//
// Created by rmqi on 8/7/18.
//

#ifndef RN_CALLBACKS_H
#define RN_CALLBACKS_H

#include <functional>
#include <memory>

namespace RN {
    class TcpConnection;

    class InetAddress;

    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

    typedef std::function<void()> TimerCallback;
    typedef std::function<
            void(const TcpConnectionPtr &)
    > ConnectionCallback;
    typedef std::function<
            void(const TcpConnectionPtr &,
                 const char *data,
                 ssize_t len)
    > MessageCallback;
    typedef std::function<void(int sockfd,
                               const InetAddress)> NewConnectionCallback;
    typedef std::function<void(const TcpConnectionPtr &)> CloseCallback;
}
#endif //RN_CALLBACKS_H
