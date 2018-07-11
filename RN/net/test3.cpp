#include "EventLoop.h"
#include "EventLoopThread.h"

void runInThread() {
    printf("runInThread(): pid = %d, tid = %d\n",
           getpid(), RN::CurrentThread::tid());
}

int main() {
    printf("main(): pid = %d, tid = %d\n",
           getpid(), RN::CurrentThread::tid());

    RN::EventLoopThread loopThread;
    RN::EventLoop *loop = loopThread.startLoop();
    loop->runInLoop(runInThread);
    sleep(1);
    loop->runAfter(2, runInThread);
    sleep(3);
    loop->quit();

    printf("exit main().\n");
}
