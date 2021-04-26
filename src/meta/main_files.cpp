#include "../models/meta.h"

#include <experimental/filesystem>
#include <iostream>
#include <vector>

namespace fs = std::experimental::filesystem;

namespace {

    void Delete(const std::string &file, Meta &meta) {
        if (fs::is_regular_file(file)) {
            meta.DeleteFile(fs::canonical(file));
        } else if (fs::is_directory(file)) {
            for (const auto &entry : fs::directory_iterator(file)) {
                Delete(entry.path(), meta);
            }
        } else {
            std::cout << "Unknown file type for " << file << "\n";
        }
    }

    void Add(const std::string &file, Meta &meta) {
        if (fs::is_regular_file(file)) {
            meta.AddFile(fs::canonical(file));
        } else if (fs::is_directory(file)) {
            for (const auto &entry : fs::directory_iterator(file)) {
                Add(entry.path(), meta);
            }
        } else {
            std::cout << "Unknown file type for " << file << "\n";
        }
    }

    void DeleteFiles(const std::vector<std::string> &files) {
        Meta meta;
        for (const auto &file: files) {
            if (!fs::exists(file)) {
                std::cout << "WARNING! File " << file << " doesn't exist\n";
            }
            Delete(file, meta);
        }
        meta.SaveMeta();
    }

    void AddFiles(const std::vector<std::string> &files) {
        Meta meta;
        for (const auto &file: files) {
            if (!fs::exists(file)) {
                std::cout << "WARNING! File " << file << " doesn't exist\n";
            }
            Add(file, meta);
        }
        meta.SaveMeta();
    }
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        std::cout << "Wrong format! Template of this command is 'add/delete *files*'\n";
        return 0;
    }
    std::string action = argv[1];
    if (action != "delete" && action != "add") {
        std::cout
                << "Wrong format! Second arg should be 'add' or 'delete'. Template of this command is 'add/delete *files*'\n";
        return 0;
    }
    if (argc == 2) {
        std::cout << "Wrong format! There are no files to " << action
                  << ". Template of this command is 'add/delete *files*'\n";
        return 0;
    }

    std::vector<std::string> files;
    for (int i = 2; i < argc; ++i) {
        files.push_back(argv[i]);
    }

    if (action == "delete") {
        DeleteFiles(files);
    } else {
        AddFiles(files);
    }
    std::cout << "Finised! " << std::endl;
}
