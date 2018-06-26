add_executable(ThreadSingletonTest ThreadSingletonTest.cpp)
target_link_libraries(ThreadSingletonTest RN_base)

add_executable(ThreadPoolTest ThreadPoolTest.cpp)
target_link_libraries(ThreadPoolTest RN_base)


add_executable(MutexTest MutexTest.cpp)
target_link_libraries(MutexTest RN_base)
add_executable(BlockQueueTest BlockQueueTest.cpp)
target_link_libraries(BlockQueueTest RN_base)

add_executable(ThreadTest ThreadTest.cpp)
target_link_libraries(ThreadTest RN_base)