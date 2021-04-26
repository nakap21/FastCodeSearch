cmake_minimum_required(VERSION 3.17)
project(FastCodeSearch)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src/start)
add_subdirectory(src/search)
add_subdirectory(src/meta)
add_subdirectory(src/models)
add_subdirectory(src/libs/boost-cmake)
add_subdirectory(src/libs/re2)

find_package(Threads)

target_link_libraries(models Boost::filesystem Boost::boost Boost::serialization Threads::Threads re2::re2 stdc++fs)
target_link_libraries(start models)
target_link_libraries(search models)
target_link_libraries(configs models)
target_link_libraries(files models)

