message(STATUS "protobuf dir")
add_library(proto SHARED)
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
set(PROTO_SRCS "${CMAKE_CURRENT_BINARY_DIR}adressbook.pb.cc")
set(PROTO_HDRS "${CMAKE_CURRENT_BINARY_DIR}adressbook.pb.h")

file(GLOB files "${CMAKE_CURRENT_LIST_DIR}/*.proto")
message(STATUS "proto files ${files}")

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${files})

target_sources(proto
    PRIVATE
    ${PROTO_SRCS} ${PROTO_HDRS})

install(
    TARGETS
    proto
    LIBRARY DESTINATION
    ${CMAKE_INSTALL_PREFIX}/lib/dlplan)

install(
    DIRECTORY
    ${PROJECT_SOURCE_DIR}/include/dlplan/
    DESTINATION
    ${CMAKE_INSTALL_PREFIX}/include/dlplan)