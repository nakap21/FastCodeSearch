add_executable(search main.cpp search.cpp)

set_target_properties(search
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "../../build"
        LIBRARY_OUTPUT_DIRECTORY "../../build"
        RUNTIME_OUTPUT_DIRECTORY "../../build"
        )
