cmake_minimum_required(VERSION 3.16)

enable_language(CXX)
find_package(Threads)

project (bexpect)

add_executable(bexpect "${CMAKE_SOURCE_DIR}/source/bexpect.cpp")

add_custom_target(blang ALL COMMAND cd ${CMAKE_SOURCE_DIR}/blang && make lib)

add_custom_target(server ALL COMMAND cd ${CMAKE_SOURCE_DIR}/test && make)

target_sources(bexpect PUBLIC "${CMAKE_SOURCE_DIR}/source/bexpect.cpp" "${CMAKE_SOURCE_DIR}/source/bsocket.cpp")

include_directories("${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/blang/include")

link_directories("${CMAKE_SOURCE_DIR}/blang")

set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -O3 -pthread -DNDEBUG")

target_link_libraries(bexpect PUBLIC ${FLEX_LIBRARIES} ${CMAKE_SOURCE_DIR}/blang/libblang.a)
