include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        all_tests
        regex_query_test.cpp
        meta_test.cpp
        search_test.cpp
        ../src/search/search.cpp
        ../src/search/regex.cpp
        ../src/meta/configs.cpp
        ../src/meta/files.cpp
)

target_link_libraries(
        all_tests
        gtest
        gtest_main
)

include(GoogleTest)

gtest_discover_tests(all_tests)

file(COPY static DESTINATION ${CMAKE_CURRENT_BINARY_DIR})