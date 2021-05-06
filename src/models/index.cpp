#include "index.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/detail/common_iarchive.hpp>
#include <experimental/filesystem>
#include <fstream>
#include <future>

namespace fs = std::experimental::filesystem;

using namespace boost::archive;

void Index::AddFile(int file_id, const Meta &meta) {
    const auto &str = meta.GetPathById(file_id);
    std::ifstream in(str);
    char a, b, c;
    if (!in.get(a) || !in.get(b)) {
        // file is too short for code
        in.close();
        return;
    }
    while (in.get(c)) {
        int tr = std::tolower(a) * 256 * 256 + std::tolower(b) * 256 + std::tolower(c);
        index[tr].insert(file_id);
        a = b;
        b = c;
    }
    in.close();
}

void Index::DeleteFile(int file_id) {
    for (auto &now: index) {
        now.second.erase(file_id);
    }
}

Index::IndexForSearch Index::ConvertForSave() const {
    std::vector<std::pair<int, std::vector<int>>> pre;
    for (const auto &now: index) {
        pre.push_back({now.first, {}});
        int index = pre.size() - 1;
        for (const auto &file_id: now.second) {
            pre[index].second.push_back(file_id);
        }
        std::sort(pre[index].second.begin(), pre[index].second.end());
    }
    std::sort(pre.begin(), pre.end(), [](auto &left, auto &right) {
        return left.first < right.first;
    });

    IndexForSearch result;
    for (int i = 0; i < pre.size(); ++i) {
        result.trigrams.push_back(pre[i].first);
        result.files_ids.push_back(pre[i].second);
    }
    return result;
}

std::string GetIndexFileName(int index) {
    return "index" + std::to_string(index) + ".bin";
}

Index::IndexForSearch LoadIndex(const std::string &file_path) {
    try {
        Index::IndexForSearch index;
        std::ifstream file{file_path};

//    Proto way
//    Message m;
//    m.ParseFromIstream(&file);
//    index.trigrams.reserve(m.trigrams().size());
//    index.trigrams = {m.trigrams().begin(), m.trigrams().end()};
//    for (const auto& files: m.files()) {
//        index.files_ids.push_back({files.file().begin(), files.file().end()});
//    }

        binary_iarchive ia{file};
        ia >> index;
        file.close();
        return index;
    } catch (const std::exception &ex) {
        auto err = std::string(ex.what());
        throw std::runtime_error("Can't load index " + file_path + ": " + err);
    }
}

void SaveIndex(const std::string &file_name, const Index &index) {
    try {
        auto index_for_save = index.ConvertForSave();
//    Proto way
//    Message m;
//    for (const auto& tr: index_for_save.trigrams) {
//        m.add_trigrams(tr);
//    }
//    for (const auto& files: index_for_save.files_ids) {
//        Files* f =try m.add_files();;
//        for (const auto& file: files) {
//            f->add_file(file);
//        }
//    }
//    m.SerializeToOstream(&file);

        std::ofstream file{file_name};
        binary_oarchive oa{file};
        oa << index_for_save;
    } catch (const std::exception &ex) {
        auto err = std::string(ex.what());
        throw std::runtime_error("Can't save index " + file_name + ": " + err);
    }
}

void SaveIndexes(const std::vector<Index> &indexes) {
    for (size_t i = 0; i < indexes.size(); ++i) {
        SaveIndex(GetIndexFileName(i), indexes[i]);
    }
}

std::vector<Index::IndexForSearch> LoadIndexes(size_t cnt_indexes, const std::string &path) {
    std::vector<std::future<Index::IndexForSearch>> fut_index;
    for (size_t i = 0; i < cnt_indexes; ++i) {
        auto file_path = path + GetIndexFileName(i);
        fut_index.push_back(std::async(std::launch::async, [file_path] { return LoadIndex(file_path); }));
    }
    std::vector<Index::IndexForSearch> result;
    for (size_t i = 0; i < cnt_indexes; ++i) {
        result.push_back(fut_index[i].get());
    }
    return result;
}

void DeleteIndex(const std::string &file_path) {
    try {
        fs::remove(file_path);
    } catch (const std::exception &ex) {
        auto err = std::string(ex.what());
        throw std::runtime_error("Can't delete index " + file_path + ": " + err);
    }
}