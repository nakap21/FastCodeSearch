#include "../models/meta.h"
#include "../models/shard.h"

#include <chrono>
#include <sys/stat.h>
#include <thread>
#include <unordered_set>

#include <iostream>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/detail/common_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/boost_unordered_set.hpp>
#include <boost/serialization/boost_unordered_map.hpp>
#include <boost/serialization/unordered_set.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <fstream>
#include <time.h>

#include <boost/unordered_map.hpp>

//#include "test.pb.h"
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
//        files_in_meta = meta.GetFiles();
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
    }
}

struct Test {
//    boost::unordered::unordered_map<int, boost::unordered::unordered_set<int>> val;
    std::unordered_map<int, std::unordered_set<int>> val;
//    boost::container::flat_map<int, boost::container::flat_set<int>> val;
private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &archive, const unsigned int version) {
        archive & BOOST_SERIALIZATION_NVP(val);
    }
};

void SaveT(const Test &test) {
    std::ofstream file{"test.bin"};
    binary_oarchive oa{file};
    oa << test;
}

void LoadT() {
    clock_t start = clock();
    std::ifstream file{"test.bin"};
    clock_t end = clock();
    double seconds = (double) (end - start) / CLOCKS_PER_SEC;
    std::cout << "HER1 " << seconds << std::endl;
    binary_iarchive ia{file};
    Test a;
    clock_t end2 = clock();
    double seconds2 = (double) (end2 - start) / CLOCKS_PER_SEC;
    std::cout << "HER2 " << seconds2 << std::endl;
    ia >> a;
    clock_t end3 = clock();
    double seconds3 = (double) (end3 - start) / CLOCKS_PER_SEC;
    std::cout << "HER3 " << seconds3 << std::endl;
}

int main() {
//    Test a;
//    clock_t start = clock();
//    std::cout << "NOW " << std::endl;
//    for (int i = 0; i < 10000; ++i) {
//        for (int j = 0; j < 100; ++j) {
//            a.val[i].insert(j);
//        }
//    }
//    clock_t end = clock();
//    double seconds = (double) (end - start) / CLOCKS_PER_SEC;
//    std::cout << "MAke data " << seconds << std::endl;
//    std::cout << "NOW2 " << std::endl;
//    SaveT(a);
//    std::cout << "NOW3 " << std::endl;
//
//    LoadT();



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
