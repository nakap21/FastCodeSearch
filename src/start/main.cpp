#include "../models/meta.h"
#include "../models/shard.h"

#include <chrono>
#include <sys/stat.h>
#include <thread>
#include <unordered_set>

#include <iostream>

#include <time.h>
#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>
#include <boost/unordered_map.hpp>


using namespace boost::archive;

namespace {

    Shards CreateIndex(const Meta &meta) {
        Shards shards;
        for (const auto &file: meta.GetFiles()) {
            shards.AddFile(file.first, meta);
        }
        shards.SaveShards();
        return shards;
    }

    bool ShouldUpdateMeta(const Meta &meta) {
        Meta cur_meta;
        return cur_meta.GetUpdateIntervalSec().value != meta.GetUpdateIntervalSec().value ||
               cur_meta.GetCntFilesInShard().value != meta.GetCntFilesInShard().value || cur_meta.ShouldStopEngine();
    }

    void UpdateMeta(Meta &meta) {
        Meta cur_meta;
        if (cur_meta.ShouldStopEngine()) {
            meta.StopEngine();
        }
        meta.SetUpdateIntervalSec(cur_meta.GetUpdateIntervalSec().value);
        meta.SetCntFilesInShard(cur_meta.GetCntFilesInShard().value);
        meta.SaveMeta();
        meta.SaveFilePathsById();
    }

    bool ShouldUpdateIndex(const Meta &meta) {
        Meta cur_meta;
        for (const auto &file: meta.GetFiles()) {
            auto &file_path = meta.GetPathById(file.first);
            auto it = std::find(cur_meta.GetPaths().begin(), cur_meta.GetPaths().end(), file_path);
            if (it == cur_meta.GetPaths().end()) {
                return true;
            }
            struct stat fileInfo;
            stat(file_path.c_str(), &fileInfo);
            if (file.second != fileInfo.st_mtim.tv_sec) {
                return true;
            }
        }
        return meta.GetFiles().size() != cur_meta.GetFiles().size();
    }

    void UpdateIndex(Shards &shards, Meta &meta) {
        Meta cur_meta;
        std::unordered_set<std::string> files_to_add;
        std::cout << "SIZE NEW FILES = " << cur_meta.GetFiles().size() << "\n";
        for (const auto &file: cur_meta.GetFiles()) {
            files_to_add.insert(cur_meta.GetPathById(file.first));
        }
        Meta saved_meta = meta;
        for (const auto &file: saved_meta.GetFiles()) {
            auto &file_path = saved_meta.GetPathById(file.first);
            auto it = std::find(cur_meta.GetPaths().begin(), cur_meta.GetPaths().end(), file_path);
            files_to_add.erase(file_path);
            if (it == cur_meta.GetPaths().end()) {
                meta.DeleteFile(file_path);
                shards.DeleteFile(file.first);
                continue;
            }
            struct stat fileInfo;
            stat(file_path.c_str(), &fileInfo);
            if (file.second != fileInfo.st_mtim.tv_sec) {
                meta.DeleteFile(file_path);
                shards.DeleteFile(file.first);
                auto new_path_id = meta.AddFile(file_path);
                shards.AddFile(new_path_id, meta);
                continue;
            }
        }
        for (const auto &file: files_to_add) {
            std::cout << "add file " << file << "\n";
            auto new_path_id = meta.AddFile(file);
            clock_t start = clock();
            shards.AddFile(new_path_id, meta);
            clock_t end = clock();
            double seconds = (double) (end - start) / CLOCKS_PER_SEC;
            std::cout << "added file " << seconds << std::endl;
        }
        shards.SaveShards();
        meta.SaveMeta();
        meta.SaveFilePathsById();
    }
}

int main() {
    Meta meta;
    auto shards = CreateIndex(meta);
    while (!meta.ShouldStopEngine()) {
        std::cout << "GO AGAIN\n";
        if (ShouldUpdateMeta(meta)) {
            std::cout << "Update Meta\n";
            UpdateMeta(meta);
            std::cout << "FINISHED Update Meta\n";
            shards.Clear();
            shards = CreateIndex(meta);
        }
        if (ShouldUpdateIndex(meta)) {
            std::cout << "Update INDEX\n";
            clock_t start = clock();
            UpdateIndex(shards, meta);
            clock_t end = clock();
            double seconds = (double) (end - start) / CLOCKS_PER_SEC;
            std::cout << "FINISHED Update INDEX " << seconds << "\n";
        }
        std::cout << "SLEEP\n";
        std::this_thread::sleep_for(std::chrono::seconds(meta.GetUpdateIntervalSec().value));
    }
    shards.Clear();
    meta.Clear();
}