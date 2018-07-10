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
add_executable(TimestampTest TimestampTest.cpp)
target_link_libraries(TimestampTest RN_base)
add_executable(LoggingTest LoggingTest.cpp)
target_link_libraries(LoggingTest RN_base)
add_executable(Async Async.cpp)
target_link_libraries(Async RN_base)


if (BOOSTTEST_LIBRARY)
    add_executable(LogStreamTest LogStreamTest.cpp)
    target_link_libraries(LogStreamTest RN_base boost_unit_test_framework)
    add_test(NAME LogStreamTest COMMAND LogStreamTest)
endif ()

