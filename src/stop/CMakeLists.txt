set(CMAKE_CXX_STANDARD 17)

add_executable(stop main.cpp)

set_target_properties(stop
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "../../build"
        LIBRARY_OUTPUT_DIRECTORY "../../build"
        RUNTIME_OUTPUT_DIRECTORY "../../build"
        )
