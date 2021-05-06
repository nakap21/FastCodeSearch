#include "meta.h"

#include <boost/unordered_set.hpp>
#include <filesystem>
#include <fstream>
#include <sys/stat.h>
#include <optional>

using namespace boost::archive;

namespace fs = std::filesystem;

namespace {
    std::optional<int> GetPathId(const std::string &path, std::vector<std::string> &file_paths) {
        auto it = std::find(file_paths.begin(), file_paths.end(), path);
        if (it != file_paths.end()) {
            return it - file_paths.begin();
        }
        return std::nullopt;
    }

    int GetOrCreatePathId(const std::string &path, std::vector<std::string> &file_paths) {
        auto cur_path_id = GetPathId(path, file_paths);
        if (cur_path_id) {
            return *cur_path_id;
        }

        auto it = std::find(file_paths.begin(), file_paths.end(), "");
        if (it != file_paths.end()) {
            int ind = it - file_paths.begin();
            file_paths[ind] = path;
            return ind;
        }
        file_paths.push_back(path);
        return file_paths.size() - 1;
    }
}

Meta::Meta(const std::string &file_path) {
    std::ifstream file{file_path};
    try {
        binary_iarchive ia{file};
        ia >> *this;
    } catch (const std::exception &ex) {
//        It's ok. Use default settings
    }
}

void Meta::SaveMeta() {
    try {
        std::ofstream file{"meta_info.bin"};
        binary_oarchive oa{file};
        oa << *this;
    } catch (const std::exception &ex) {
        auto err = std::string(ex.what());
        throw std::runtime_error("Can't save meta_info.bin: " + err);
    }
}

int Meta::AddFile(const std::string &file) {
    auto path_id = GetOrCreatePathId(file, file_paths);
    struct stat fileInfo;
    stat(file.c_str(), &fileInfo);
    files[path_id] = fileInfo.st_mtim.tv_sec;
    return path_id;
}

void Meta::DeleteFile(const std::string &file) {
    auto path_id = GetPathId(file, file_paths);
    if (path_id) {
        files.erase(*path_id);
        file_paths[*path_id] = "";
    }
}

void Meta::SetUpdateIntervalSec(int new_value) {
    update_interval_sec.value = new_value;
}

void Meta::SetCntFilesInShard(int new_value) {
    cnt_files_in_shard.value = new_value;
}

void Meta::SetMaxSizeIndexFile(int new_value) {
    max_size_index_file.value = new_value;
}

void Meta::SetFilesFormatsIgnore(const unordered_set<std::string> &new_value) {
    files_formats_ignore.value = new_value;
}

void Meta::Clear() {
    try {
        fs::remove("meta_info.bin");
    } catch (const std::exception &ex) {
        auto err = std::string(ex.what());
        throw std::runtime_error("Can't remove meta_info.bin: " + err);
    }
    try {
        fs::remove("files_path_by_id.bin");
    } catch (const std::exception &ex) {
        auto err = std::string(ex.what());
        throw std::runtime_error("Can't remove files_path_by_id.bin: " + err);
    }
}

void Meta::StopEngine() {
    should_stop_engine = true;
}

void Meta::SaveFilePathsById() {
    try {
        std::ofstream file{"files_path_by_id.bin"};
        binary_oarchive oa{file};
        oa << file_paths;
    } catch (const std::exception &ex) {
        auto err = std::string(ex.what());
        throw std::runtime_error("Can't save files_path_by_id.bin: " + err);
    }
}

std::vector<std::string> LoadFilePathsById(const std::string &file_path) {
    try {
        std::ifstream file{file_path};
        binary_iarchive ia{file};
        std::vector<std::string> file_paths_by_id;
        ia >> file_paths_by_id;
        return file_paths_by_id;
    } catch (const std::exception &ex) {
        auto err = std::string(ex.what());
        throw std::runtime_error("Can't load " + file_path + ": " + err);
    }

}
