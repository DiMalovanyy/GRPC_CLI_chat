cmake_minimum_required(VERSION 3.5)
project(Chat LANGUAGES C CXX VERSION 0.0.1)


set(SOURCE_DIR ${CMAKE_SOURCE_DIR})
set(SERVER_IP "localhost:50051")
set(DATA_BASE_IP "localhost:27017")

foreach(subdirectory
            client server)

configure_file(
    CMAKE_CONSTANTS.h.in
        ${CMAKE_SOURCE_DIR}/${subdirectory}/CMAKE_CONSTANTS.h
)

configure_file(
    protos/chat.proto
    ${CMAKE_SOURCE_DIR}/${subdirectory}/protos/chat.proto
)
endforeach()



configure_file(
    CMAKE_CONSTANTS.h.in
    ${CMAKE_SOURCE_DIR}/client/CMAKE_CONSTANTS.h
)
configure_file(
    CMAKE_CONSTANTS.h.in
    ${CMAKE_SOURCE_DIR}/server/CMAKE_CONSTANTS.h
)

add_subdirectory(client)
add_subdirectory(server)
