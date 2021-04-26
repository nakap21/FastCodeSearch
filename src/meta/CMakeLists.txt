add_executable(files main_files.cpp)
add_executable(configs main_configs.cpp)

set_target_properties(files
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "../../build"
        LIBRARY_OUTPUT_DIRECTORY "../../build"
        RUNTIME_OUTPUT_DIRECTORY "../../build"
        )

set_target_properties(configs
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "../../build"
        LIBRARY_OUTPUT_DIRECTORY "../../build"
        RUNTIME_OUTPUT_DIRECTORY "../../build"
        )