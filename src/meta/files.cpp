#include "files.h"

#include <iostream>

namespace {
    bool IsNeededToAdd(const std::string &file, Meta &meta) {
        return fs::file_size(file) < meta.GetMaxSizeIndexFile().value &&
               meta.GetFilesFormatsIgnore().value.find(fs::path(file).extension()) ==
               meta.GetFilesFormatsIgnore().value.end();
    }

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
        if (fs::is_regular_file(file) && IsNeededToAdd(file, meta)) {
            meta.AddFile(fs::canonical(file));
        } else if (fs::is_directory(file) && !isHidden(file)) {
            for (const auto &entry : fs::directory_iterator(file)) {
                Add(entry.path(), meta);
            }
        } else {
            std::cout << "Unknown file type for " << file << "\n";
        }
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