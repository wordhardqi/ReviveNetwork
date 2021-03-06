﻿//
// Created by rmqi on 12/7/18.
//

#include "Callbacks.h"
#include "TcpConnection.h"
#include "TcpServer.h"
#include <RN/base/ProcessInfo.h>

RN::string message1;
RN::string message2;

void onConnection(const RN::TcpConnectionPtr &conn) {
    if (conn->connected()) {
        printf("onConnection(): new connection[%s] from %s \n",
               conn->getName().c_str(), conn->getPeerAddr().toHostPort().c_str());
        conn->send(message1);
        conn->send(message2);
        conn->shutdown();
    } else {
        printf("onConnection(): connection[%s] is down",
               conn->getName().c_str());
    }

}

void onMessage(const RN::TcpConnectionPtr &conn, RN::Buffer *data, RN::Timestamp receivedTime) {
    ssize_t readable = data->readableBytes();
    printf("onMessage: received %zd bytes from connection[%s], %s\n",
           readable, conn->getName().c_str(), data->retirveAllAsString().c_str());
}

int main() {
    printf("main(): pid = %d\n", getpid());

    int len1 = 100;
    int len2 = 200;
    message1.resize(len1);
    message2.resize(len2);
    std::fill(message1.begin(), message1.end(), 'A');
    std::fill(message2.begin(), message2.end(), 'B');

    RN::InetAddress listenAddr(9981);
    RN::EventLoop loop;
    RN::TcpServer server("TestConnection", &loop, listenAddr);
    server.setMessageCallback(onMessage);
    server.setConnectionCallback(onConnection);
    server.start();
    loop.loop();

}