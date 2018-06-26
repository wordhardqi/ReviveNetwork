#include <RN/base/BlockingQueue.h>
#include <RN/base/CountDownLatch.h>
#include <RN/base/Thread.h>

#include <memory>
#include <vector>
#include <unistd.h>

using std::placeholders::_1;

class Test {
 public:
  Test(int numThreads)
      : latch_(numThreads) {
    for (int i = 0; i < numThreads; ++i) {
      char name[32];
      snprintf(name, sizeof name, "work thread %d", i);
      threads_.emplace_back(new RN::Thread(
          std::bind(&Test::threadFunc, this), std::string(name)));
    }
    for (auto &thr : threads_) {
      thr->start();
    }
  }

  void run(int times) {
    printf("waiting for count down latch\n");
    latch_.wait();
    printf("all threads started\n");
    for (int i = 0; i < times; ++i) {
      char buf[32];
      snprintf(buf, sizeof buf, "hello %d", i);
      queue_.put(buf);
      printf("tid=%d, put data = %s, size = %zd\n", RN::CurrentThread::tid(), buf, queue_.size());
    }
  }

  void joinAll() {
    for (size_t i = 0; i < threads_.size(); ++i) {
      queue_.put("stop");
    }

    for (auto &thr : threads_) {
      thr->join();
    }
  }

 private:

  void threadFunc() {
    printf("tid=%d, %s started\n",
           RN::CurrentThread::tid(),
           RN::CurrentThread::name());

    latch_.countDown();
    bool running = true;
    while (running) {
      std::string d(queue_.take());
      printf("tid=%d, get data = %s, size = %zd\n", RN::CurrentThread::tid(), d.c_str(), queue_.size());
      running = (d != "stop");
    }

    printf("tid=%d, %s stopped\n",
           RN::CurrentThread::tid(),
           RN::CurrentThread::name());
  }

  RN::BlockingQueue<std::string> queue_;
  RN::CountDownLatch latch_;
  std::vector<std::unique_ptr<RN::Thread>> threads_;
};

void testMove() {
  RN::BlockingQueue<std::unique_ptr<int>> queue;
  queue.put(std::unique_ptr<int>(new int(42)));
  std::unique_ptr<int> x = queue.take();
  printf("took %d\n", *x);
  *x = 123;
  queue.put(std::move(x));
  std::unique_ptr<int> y = queue.take();
  printf("took %d\n", *y);
}

int main() {
  printf("pid=%d, tid=%d\n", ::getpid(), RN::CurrentThread::tid());
  Test t(5);
  t.run(100);
  t.joinAll();

  testMove();

  printf("number of created threads %d\n", RN::Thread::numCreated());
}
