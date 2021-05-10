cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0079 NEW)
project(FastCodeSearch CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

add_subdirectory(src/start)
add_subdirectory(src/search)
add_subdirectory(src/meta)
add_subdirectory(src/models)
add_subdirectory(tests)
add_subdirectory(benchmark)
add_subdirectory(src/libs/abseil-cpp)
add_subdirectory(src/libs/boost-cmake)
add_subdirectory(src/libs/re2)
add_subdirectory(proto)

find_package(Threads REQUIRED)

target_link_libraries(models
        proto
        ${PROTOBUF_LIBRARIES}
        absl::base
        absl::flat_hash_map
        Boost::boost
        Boost::serialization
        ${CMAKE_THREAD_LIBS_INIT}
        "$<$<CXX_COMPILER_ID:GNU>:-pthread>$<$<CXX_COMPILER_ID:Clang>:-pthreads>"
        Threads::Threads
        re2::re2
        stdc++fs
        )
target_link_libraries(start models)
target_link_libraries(stop models)
target_link_libraries(search models)
target_link_libraries(configs models)
target_link_libraries(files models)
target_link_libraries(all_tests models)
target_link_libraries(search_benchmark models)

set_target_properties(start stop search configs files models
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/build"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/build"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/build"
        )


