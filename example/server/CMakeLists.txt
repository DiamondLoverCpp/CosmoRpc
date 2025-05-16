add_executable(server user_service.cc ../user.pb.cc)
target_link_libraries(server cosmorpc protobuf)