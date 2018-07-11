#include "Acceptor.h"

void newConnection(int sockfd, const RN::InetAddress &peerAddr) {
    printf("newConnection(): accepted a new connection from %s\n",
           peerAddr.toHostPort().c_str());
    ::write(sockfd, "How are you?\n", 13);
    RN::sockets::close(sockfd);
}

void newConnection2(int sockfd, const RN::InetAddress &peerAddr) {
    printf("newConnection2(): accepted a new connection from %s\n",
           peerAddr.toHostPort().c_str());
    ::write(sockfd, "Fine\n", 5);
    RN::sockets::close(sockfd);
}

int main() {
    printf("main(): pid = %d\n", getpid());

    RN::InetAddress listenAddr(9981);
    RN::InetAddress listenAddr2(9982);

    RN::EventLoop loop;

    RN::Acceptor acceptor(&loop, listenAddr);
    RN::Acceptor acceptor2(&loop, listenAddr2);

    acceptor.setNewConnectionCallback(newConnection);
    acceptor2.setNewConnectionCallback(newConnection2);
    acceptor.listen();
    acceptor2.listen();

    loop.loop();
}
