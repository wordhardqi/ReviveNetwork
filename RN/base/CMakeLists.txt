set(base_SRCS
        Condition.cpp
        CountDownLatch.cpp
        Thread.cpp
        )

add_library(RN_base ${base_SRCS})
target_link_libraries(RN_base pthread rt)

#add_library(muduo_base_cpp11 ${base_SRCS})
#target_link_libraries(muduo_base_cpp11 pthread rt)
#set_target_properties(muduo_base_cpp11 PROPERTIES COMPILE_FLAGS "-std=c++0x")

install(TARGETS RN_base DESTINATION lib)
#install(TARGETS muduo_base_cpp11 DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/RN/base)

add_subdirectory(tests)

#if(NOT CMAKE_BUILD_NO_EXAMPLES)
#    add_subdirectory(tests)
#endif()
