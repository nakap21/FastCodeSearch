include(FetchContent)
FetchContent_Declare(googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark
        )
FetchContent_MakeAvailable(googlebenchmark)

add_executable(
        search_benchmark
        test.cpp
        ../src/search/regex.cpp
        ../src/search/search.cpp
        ../src/models/meta.cpp
        ../src/models/index.cpp
        ../src/models/shard.cpp
        ../src/start/updates.cpp
)

target_link_libraries(
        search_benchmark
        benchmark::benchmark
        benchmark::benchmark_main
)

file(COPY static DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
