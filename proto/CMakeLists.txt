include(FetchContent)
FetchContent_Declare(
        protobuf
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/protocolbuffers/protobuf/archive/refs/tags/v3.15.8.tar.gz
)
FetchContent_MakeAvailable(protobuf)

include(FindProtobuf)

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIR})

PROTOBUF_GENERATE_CPP(PROTO_SRC PROTO_HEADER message.proto)
ADD_LIBRARY(proto ${PROTO_HEADER} ${PROTO_SRC})

target_link_libraries(proto ${PROTOBUF_LIBRARIES})
