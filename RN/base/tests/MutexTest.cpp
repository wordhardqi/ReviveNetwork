//
// Created by renming on 6/22/18.
//

#include <vector>
#include <stdio.h>
#include <RN/base/Atomic.h>
#include <RN/base/Mutex.h>
#include <cstdlib>
#include <RN/base/Thread.h>
#include <memory>
using namespace RN;
using namespace std;
MutexLock g_mutex;
vector<int> g_vec;
const int kCount = 10 * 100 * 1000;

void threadFunc() {
  for (int i = 0; i < kCount; ++i) {
    MutexLockGuard lockGuard(g_mutex);
    g_vec.push_back(1);
  }
}
int foo() __attribute__ ((noinline));
int g_count = 0;
int foo() {
  MutexLockGuard lockGuard(g_mutex);
  if (!g_mutex.isLockedByThisThread()) {
    printf("Fail \n");
    return -1;
  }
  ++g_count;
  return 0;
}
int main() {
  MCHECK(foo());
  if (g_count != 1) {
    printf("MCHECK calls twice. \n");
    abort();
  }
  const int kMaxThreads = 7;
  g_vec.reserve(kMaxThreads * kCount);
  for (int i = 0; i < kCount; ++i) {
    g_vec.push_back(i);

  }
  threadFunc();
  for (int nthreads = 1; nthreads < kMaxThreads; ++nthreads) {
    vector<unique_ptr<Thread>> threads;
    g_vec.clear();
    for (int i = 0; i < nthreads; ++i) {
      threads.emplace_back(new Thread(&threadFunc));
      threads.back()->start();
    }
    for (int i = 0; i < nthreads; ++i) {
      threads[i]->join();
    }
    fprintf(stderr, "%lu", g_vec.size());
  }

}