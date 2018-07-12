//
// Created by rmqi on 11/7/18.
//

#ifndef RN_ACCEPTOR_H
#define RN_ACCEPTOR_H

#include <RN/base/noncopyable.h>
#include <functional>
#include "InetAddress.h"
#include "EventLoop.h"
#include "Socket.h"

namespace RN {
    class Acceptor : RN::noncopyable {
    public:
        typedef std::function<void(int sockfd,
                                   const InetAddress)> NewConnectionCallback;

        Acceptor(EventLoop *loop, const InetAddress &addr);

        void setNewConnectionCallback(const NewConnectionCallback cb) {
            newConnectionCallback_ = cb;
        }

        bool listennning() {
            return listennning_;
        }

        void listen();

    private:
        void handleRead();

        EventLoop *loop_;
        Socket acceptSocket_;
        Channel acceptChannel_;
        bool listennning_;
        NewConnectionCallback newConnectionCallback_;
    };

}


#endif //RN_ACCEPTOR_H
