set(BINARY ${CMAKE_PROJECT_NAME}_server)
set(HEADERS
    MongoDB_Manager.h
    server_operator.h
    data_base_service.h
    utility.h
    json_parser.h
    chat_service.h
	)
set(SOURCES
	main.cpp
    json_parser.cpp
	)
 
 
find_package(Boost REQUIRED)
find_package(mongocxx REQUIRED)
 
find_package(Threads REQUIRED)
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
if(CMAKE_CROSSCOMPILING)
    find_program(_PROTOBUF_PROTOC protoc)
else()
    set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
endif()

find_package(gRPC CONFIG REQUIRED)
set(_GRPC_GRPCPP gRPC::grpc++)
if(CMAKE_CROSSCOMPILING)
    find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
endif()

get_filename_component(chat_proto "${CMAKE_CURRENT_SOURCE_DIR}/protos/chat.proto" ABSOLUTE)
get_filename_component(chat_proto_path "${chat_proto}" PATH)

set(chat_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/chat.pb.cc")
set(chat_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/chat.pb.h")
set(chat_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/chat.grpc.pb.cc")
set(chat_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/chat.grpc.pb.h")
add_custom_command(
      OUTPUT "${chat_proto_srcs}" "${chat_proto_hdrs}" "${chat_grpc_srcs}" "${chat_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${chat_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${chat_proto}"
      DEPENDS "${chat_proto}")

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

include_directories(${Boost_INCLUDE_DIRS})
 
add_executable(${BINARY} ${HEADERS} ${SOURCES} ${chat_grpc_srcs} ${chat_proto_srcs})
set_target_properties(${BINARY} PROPERTIES
                        CXX_STANDARD 17
                        CXX_STANDARD_REQUIRED ON
)

target_link_libraries(${BINARY} ${Boost_LIBRARIES})
target_link_libraries(${BINARY} mongo::mongocxx_shared)

target_link_libraries(${BINARY}
                    ${_REFLECTION}
                    ${_GRPC_GRPCPP}
                    ${_PROTOBUF_LIBPROTOBUF})
