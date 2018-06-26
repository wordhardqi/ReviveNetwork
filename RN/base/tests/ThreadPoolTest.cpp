//
// Created by renming on 6/22/18.
//

//#include <string>
//#include <RN/base/CurrentThread.h>
//#include <RN/base/Thread.h>
//#include <unistd.h>
//#include <RN/base/ThreadPool.h>
//
//// Todo:: Add a simple test for Threadpool.
//
//void print(int taskSeqNumber){
//  fprintf(stderr,"tid %d is running task %d \n", RN::CurrentThread::tid(), taskSeqNumber);
//}
//void print_void(){
//
//}
//void test(int queueSize){
//  RN::ThreadPool threadPool(string("RNThreadPool"),queueSize);
//  threadPool.setMaxQueueSize(queueSize);
//  threadPool.start(3);
//  threadPool.post(std::bind(&print,1));
//  threadPool.post(std::bind(&print,2));
//
//  threadPool.stop();
//
//}
//int main() {
//  test(5);
//}

#include <RN/base/ThreadPool.h>
#include <unistd.h>  // usleep

void print() {
  printf("tid=%d\n", RN::CurrentThread::tid());
  return;
}

void printString(const std::string &str) {
  fprintf(stderr, "%s", str.c_str());
  usleep(100 * 1000);
}

void test(int maxSize) {
  fprintf(stderr, "Test ThreadPool with max queue size = %d \n", maxSize);
  RN::ThreadPool pool("MainThreadPool");
  pool.setMaxQueueSize(maxSize);
  pool.start(2);

  fprintf(stderr, "Adding \n");
  pool.post(print);
  pool.post(print);
  pool.post(print);
  pool.post(print);
//  for (int i = 0; i < 100; ++i)
//  {
//    char buf[32];
//    snprintf(buf, sizeof buf, "task %d", i);
//    pool.post(std::bind(printString, std::string(buf)));
//  }
  fprintf(stderr, "Done");

  RN::CountDownLatch latch(1);
  pool.post(std::bind(&RN::CountDownLatch::countDown, &latch));
  latch.wait();
  pool.stop();

}

int main() {
//  test(0);
//  test(1);
  test(5);
//  test(10);
//  test(50);
}

