#include "../models/shard.h"

#include <chrono>
#include <sys/stat.h>
#include <thread>
#include <unordered_set>

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
               cur_meta.GetCntFilesInShard().value != meta.GetCntFilesInShard().value;
    }

    void UpdateMeta(Meta &meta) {
        Meta cur_meta;
        meta.SetUpdateIntervalSec(cur_meta.GetUpdateIntervalSec().value);
        meta.SetCntFilesInShard(cur_meta.GetCntFilesInShard().value);
        meta.SaveMeta();
    }

    bool ShouldUpdateIndex(const Meta &meta) {
        Meta cur_meta;
        for (const auto &file: meta.GetFiles()) {
            auto it = cur_meta.GetFiles().find(file.first);
            if (it == cur_meta.GetFiles().end()) {
                return true;
            }
            struct stat fileInfo;
            stat(file.first.c_str(), &fileInfo);
            if (file.second != fileInfo.st_mtim.tv_sec) {
                return true;
            }
        }
        return meta.GetFiles().size() != cur_meta.GetFiles().size();
    }

    void UpdateIndex(Shards &shards, Meta &meta) {
        Meta cur_meta;
        std::unordered_set<std::string> files_to_add;
        for (const auto &file: cur_meta.GetFiles()) {
            files_to_add.insert(file.first);
        }
        auto files_in_meta = meta.GetFiles();
        for (const auto &file: files_in_meta) {
            auto it = cur_meta.GetFiles().find(file.first);
            files_to_add.erase(file.first);
            if (it == cur_meta.GetFiles().end()) {
                meta.DeleteFile(file.first);
                shards.DeleteFile(file.first);
                continue;
            }
            struct stat fileInfo;
            stat(file.first.c_str(), &fileInfo);
            if (file.second != fileInfo.st_mtim.tv_sec) {
                meta.DeleteFile(file.first);
                shards.DeleteFile(file.first);
                meta.AddFile(file.first);
                shards.AddFile(file.first, meta);
                continue;
            }
        }
        for (const auto &file: files_to_add) {
            meta.AddFile(file);
            shards.AddFile(file, meta);
        }
        shards.SaveShards();
        meta.SaveMeta();
    }
}

int main() {
    Meta meta;
    auto shards = CreateIndex(meta);
    while (!meta.ShouldStopEngine()) {
        if (ShouldUpdateMeta(meta)) {
            UpdateMeta(meta);
        }
        if (ShouldUpdateIndex(meta)) {
            UpdateIndex(shards, meta);
        }
        std::this_thread::sleep_for(std::chrono::seconds(meta.GetUpdateIntervalSec().value));
    }
    // TODO clean logic
}
