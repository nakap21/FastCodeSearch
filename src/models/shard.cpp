#include "shard.h"

#include <experimental/filesystem>
#include <fstream>

#include <iostream>
#include <time.h>

#include <future>
using namespace boost::archive;
namespace fs = std::experimental::filesystem;

namespace {


}

void Shards::AddFile(int file_id, const Meta &meta) {
    if (files_shard.find(file_id) != files_shard.end()) {
//        already exist
        return;
    }

    auto it = std::find_if(files_cnt_on_shrads.begin(), files_cnt_on_shrads.end(),
                           [&meta](const auto &cur_cnt) {
                               return cur_cnt < meta.GetCntFilesInShard().value;
                           });
    if (it == files_cnt_on_shrads.end()) {
        files_cnt_on_shrads.push_back({1});
        int ind = files_cnt_on_shrads.size() - 1;
        files_shard[file_id] = ind;
        Index new_index;
        new_index.AddFile(file_id, meta);
        indexes.push_back(new_index);
    } else {
        (*it)++;
        int ind = it - files_cnt_on_shrads.begin();
        files_shard[file_id] = ind;
        indexes[ind].AddFile(file_id, meta);
    }
}

void Shards::DeleteFile(int file_id) {
    auto it = files_shard.find(file_id);
    if (it == files_shard.end()) {
//        no file in shards
        return;
    }
    int index = it->second;

    files_cnt_on_shrads[index]--;
    files_shard.erase(file_id);
    indexes[index].DeleteFile(file_id);
}

void Shards::SaveCntIndexes() {
    std::ofstream file{"cnt_indexes.bin"};
    binary_oarchive oa{file};
    oa << files_cnt_on_shrads.size();
}

size_t LoadCntIndexes(const std::string& file_path) {
    std::ifstream file{file_path};
    binary_iarchive ia{file};
    size_t cnt;
    ia >> cnt;
    return cnt;
}

void Shards::SaveShards() {
    SaveCntIndexes();
    SaveIndexes(indexes);
}

//std::vector<Index::IndexForSearch> Shards::LoadShards() {
////    std::cout << "KE0\n";
//    std::ifstream file{"shards.bin"};
//    binary_iarchive ia{file};
//    ia >> *this;
////    std::cout << "KEK1\n";
//    clock_t start = clock();
//    std::vector<std::future<Index::IndexForSearch>> fut_index;
//    for (size_t i = 0; i < files_cnt_on_shrads.size(); ++i) {
//        fut_index.push_back(std::async(std::launch::async, [i]{ return LoadIndex(GetIndexFileName(i)); }));
//    }
//    std::vector<Index::IndexForSearch> result;
//    for (size_t i = 0; i < files_cnt_on_shrads.size(); ++i) {
//        result.push_back(fut_index[i].get());
//    }
//    clock_t end = clock();
//    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
//    std::cout << "Load Indexes " << seconds << std::endl;
//    return result;
//}

void Shards::Clear() {
    for (size_t i = 0; i < files_cnt_on_shrads.size(); ++i) {
        auto index_file_name = GetIndexFileName(i);
        fs::remove(index_file_name);
    }
    files_cnt_on_shrads.clear();
    files_shard.clear();
    fs::remove("shards.bin");
}
