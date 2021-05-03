set(CMAKE_CXX_STANDARD 17)

add_executable(start main.cpp)

#include_directories(messages)
#find_package(Protobuf REQUIRED)

set_target_properties(start
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "../../build"
        LIBRARY_OUTPUT_DIRECTORY "../../build"
        RUNTIME_OUTPUT_DIRECTORY "../../build"
        )


