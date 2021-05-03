#include "../models/meta.h"

#include <experimental/filesystem>
#include <iostream>
#include <vector>
#include <unordered_set>

namespace fs = std::experimental::filesystem;

namespace {

    bool isHidden(const fs::path &p) {
        fs::path::string_type name = p.filename();
        if (name != ".." &&
            name != "." &&
            name[0] == '.') {
            return true;
        }
        return false;
    }

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
        if (fs::is_regular_file(file) && fs::file_size(file) < meta.GetMaxSizeIndexFile().value &&
            meta.GetFilesFormatsIgnore().value.find(fs::path(file).extension()) ==
            meta.GetFilesFormatsIgnore().value.end()) {
            std::cout << fs::path(file) << " ->>>" << fs::path(file).extension() << std::endl;
            meta.AddFile(fs::canonical(file));
        } else if (fs::is_directory(file) && !isHidden(file)) {
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
}
