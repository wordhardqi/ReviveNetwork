add_executable(test_inc
        test.cpp
        TimerQueue.cpp
        EventLoop.cpp
        Poller.cpp
        Channel.cpp
        Timer.cpp
        IOWakeupTrigger.cpp
        )
target_link_libraries(test_inc RN_base)

add_executable(test2
        test2.cpp
        TimerQueue.cpp
        EventLoop.cpp
        Poller.cpp
        Channel.cpp
        Timer.cpp
        IOWakeupTrigger.cpp
        )

add_executable(test3
        test3.cpp
        TimerQueue.cpp
        EventLoop.cpp
        Poller.cpp
        Channel.cpp
        Timer.cpp
        IOWakeupTrigger.cpp
        EventLoopThread.cpp
        )

add_executable(test7
        test7.cpp
        TimerQueue.cpp
        EventLoop.cpp
        Poller.cpp
        Channel.cpp
        Timer.cpp
        IOWakeupTrigger.cpp
        EventLoopThread.cpp
        Socket.cpp
        SocketsOps.cpp
        Acceptor.cpp
        )
target_link_libraries(test7 RN_base)

add_executable(testConnection
        testConnection.cpp
        TimerQueue.cpp
        EventLoop.cpp
        Poller.cpp
        Channel.cpp
        Timer.cpp
        IOWakeupTrigger.cpp
        EventLoopThread.cpp
        Socket.cpp
        SocketsOps.cpp
        Acceptor.cpp
        TcpConnection.cpp
        TcpServer.cpp

        )
target_link_libraries(testConnection RN_base)
add_executable(testCharGen
        testCharGen.cpp
        TimerQueue.cpp
        EventLoop.cpp
        Poller.cpp
        Channel.cpp
        Timer.cpp
        IOWakeupTrigger.cpp
        EventLoopThread.cpp
        Socket.cpp
        SocketsOps.cpp
        Acceptor.cpp
        TcpConnection.cpp
        TcpServer.cpp
        EventLoopThreadPool.cpp

        )
target_link_libraries(testCharGen RN_base)