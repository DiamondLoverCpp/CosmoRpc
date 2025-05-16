set(SRC_LIST
        rpc_application.cc
        rpc_server.cc
        rpc_config.cc
        rpc_header.pb.cc
        rpc_channel.cc
        rpc_controller.cc
        rpc_zookeeper.cc)
add_library(cosmorpc ${SRC_LIST})
target_link_libraries(cosmorpc muduo_net muduo_base pthread zookeeper_mt)