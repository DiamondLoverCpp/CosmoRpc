set(SRC_LIST user_client.cc ../user.pb.cc)

add_executable(client ${SRC_LIST})
target_link_libraries(client cosmorpc protobuf)