//
// Created by rmqi on 11/7/18.
//

#include "Acceptor.h"

RN::Acceptor::Acceptor(RN::EventLoop *loop, const RN::InetAddress &addr)
        : loop_(loop),
          acceptSocket_(sockets::createNonblockingOrDie()),
          acceptChannel_(loop, acceptSocket_.fd()),
          listennning_(false) {

    acceptSocket_.setReuseAddr(true);
    acceptSocket_.bindAddress(addr);
    acceptChannel_.setReadCallback(
            std::bind(&Acceptor::handleRead, this));
//    acceptChannel_.enableReading();

}


void RN::Acceptor::handleRead() {
    loop_->assertInLoopThread();
    InetAddress peerAddr(0);
    int connfd = acceptSocket_.accept(&peerAddr);
    if (connfd >= 0) {
        if (newConnectionCallback_) {
            newConnectionCallback_(connfd, peerAddr);
        } else {
            sockets::close(connfd);
        }
    }
}

void RN::Acceptor::listen() {
    loop_->assertInLoopThread();
    listennning_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading();
}
