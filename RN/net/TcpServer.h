﻿//
// Created by rmqi on 11/7/18.
//

#ifndef RN_TCPSERVER_H
#define RN_TCPSERVER_H

#include <bits/unique_ptr.h>
#include <map>
#include <functional>
#include "Callbacks.h"
#include "Acceptor.h"
#include "TcpConnection.h"

namespace RN {
    class TcpServer {
    public:
        typedef std::map<string, TcpConnectionPtr> ConnectionMap;

        TcpServer(const string name, EventLoop *loop, const InetAddress &serverAddr)
                : name_(name), loop_(loop), serverAddr_(serverAddr),
                  acceptor_(new Acceptor(loop_, serverAddr_)),
                  nextConnectionId_(1),
                  connections_(),
                  started_(false) {
            using namespace std::placeholders;
            acceptor_->setNewConnectionCallback(
                    std::bind(&TcpServer::createNewConneciton, this, _1, _2));

        }

        ~TcpServer() {
            loop_->quit();

        }

        void start() {
            if (!started_) {
                started_ = true;

            }
            if (!acceptor_->listennning()) {
                loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
            }

        }

        void setMessageCallback(MessageCallback cb) {
            messageCallback_ = cb;
        }

        void setConnectionCallback(ConnectionCallback cb) {
            connectionCallback_ = cb;
        }

        void setCloseCallback(CloseCallback cb) {
            closeCallback_ = cb;
        }

        const InetAddress &serverAddr() {
            return serverAddr_;
        }

    private:

        const string getConnectionName() {
            char buf[32];
            bzero(buf, sizeof(buf));
            snprintf(buf, sizeof(buf), "%s#%d", name_.c_str(), nextConnectionId_);
            return buf;
        }

        void createNewConneciton(int connfd, const InetAddress peerAddr) {
            loop_->assertInLoopThread();
            const string connName = getConnectionName();
            LOG_INFO << "TcpServer::newConnection [" << name_
                     << "] - new connection [" << connName
                     << "] from " << peerAddr.toHostPort();
            TcpConnectionPtr connPtr(
                    new TcpConnection(
                            loop_, connName, connfd, serverAddr_, peerAddr));
            connPtr->setConnectionCallback(connectionCallback_);
            connPtr->setMessageCallback(messageCallback_);
            connPtr->setCloseCallback(std::bind(
                    &TcpServer::removeConnection, this, std::placeholders::_1)
            );
            connections_[connName] = connPtr;
            ++nextConnectionId_;
            connPtr->connectionEstablished();
        }


        void removeConnection(const TcpConnectionPtr &conn) {
            loop_->assertInLoopThread();
            LOG_INFO << "TcpServer::removeConnection [" << name_
                     << "] - connection " << conn->getName();
            connections_.erase(conn->getName());

            loop_->queueInLoop(std::bind(
                    &TcpConnection::connectionDestroyed, conn));
        }

        const string name_;

        EventLoop *loop_;
        InetAddress serverAddr_;

        std::unique_ptr<Acceptor> acceptor_;
        int nextConnectionId_;
        ConnectionMap connections_;
        bool started_;
        MessageCallback messageCallback_;
        ConnectionCallback connectionCallback_;
        CloseCallback closeCallback_;


    };
}

#endif //RN_TCPSERVER_H
