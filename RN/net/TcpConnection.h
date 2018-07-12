//
// Created by rmqi on 11/7/18.
//

#ifndef RN_TCPCONNECITON_H
#define RN_TCPCONNECITON_H

#include <RN/base/Logging.h>
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Callbacks.h"

namespace RN {
    class TcpConnection : RN::noncopyable,
                          public std::enable_shared_from_this<TcpConnection> {
    public:
        TcpConnection(EventLoop *loop,
                      const string &name,
                      int socketfd,
                      const InetAddress &localAddr,
                      const InetAddress &peerAddr)
                : loop_(loop), name_(name), state_(kConnecitng),
                  socket_(new Socket(socketfd)),
                  channel_(new Channel(loop_, socketfd)),
                  localAddr_(localAddr),
                  peerAddr_(peerAddr) {
            LOG_DEBUG << "TcpConnection::ctor[" << name_
                      << "] at " << this << " fd=" << socketfd;
            channel_->setReadCallback(
                    std::bind(&TcpConnection::handleRead, this));


        }

        ~TcpConnection() {
            LOG_DEBUG << "TcpConnection::dtor[" << name_ << "] at " << this
                      << " fd=" << channel_->fd();
        }


        EventLoop *getLoop() const {
            return loop_;
        }

        const string &getName() const {
            return name_;
        }

        const InetAddress &getLocalAddr() const {
            return localAddr_;
        }

        const InetAddress &getPeerAddr() const {
            return peerAddr_;
        }

        bool connected() const {
            return state_ == kConnected;
        }


        const std::unique_ptr<Socket> &getSocket() const {
            return socket_;
        }

        const std::unique_ptr<Channel> &getChannel() const {
            return channel_;
        }

        void setConnectionCallback(const ConnectionCallback &connectionCallback) {
            connectionCallback_ = connectionCallback;
        }

        void setMessageCallback(const MessageCallback &messageCallback) {
            messageCallback_ = messageCallback;
        }

        void setCloseCallback(CloseCallback cb) {
            closeCallback_ = cb;
        }

        void connectionEstablished();

        void connectionDestroyed();

    private:
        enum StateE {
            kConnecitng,
            kConnected,
            kDisconnected,
        };

        void handleRead();

        void handleWrite();

        void handleClose();

        void handleError();

        void setState(StateE s) {
            state_ = s;
        }

        EventLoop *loop_;
        string name_;
        StateE state_;
        std::unique_ptr<Socket> socket_;
        std::unique_ptr<Channel> channel_;
        InetAddress localAddr_;
        InetAddress peerAddr_;
        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;
        CloseCallback closeCallback_;

    };
}


#endif //RN_TCPCONNECITON_H
