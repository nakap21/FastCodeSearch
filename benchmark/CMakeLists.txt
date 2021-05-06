include(FetchContent)
FetchContent_Declare(googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark
        )
FetchContent_MakeAvailable(googlebenchmark)

#find_package(benchmark REQUIRED)

add_executable(
        benchmark_tes
        test.cpp
        ../src/search/regex.cpp
        ../src/search/search.cpp
        ../src/models/meta.cpp
        ../src/models/index.cpp
        ../src/models/shard.cpp

)
target_link_libraries(
        benchmark_tes
        benchmark::benchmark
        benchmark::benchmark_main
)

#include(GoogleTest)
#gtest_discover_tests(hello_test)