#include "index.h"

#include <fstream>
#include <optional>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <time.h>

#include <iostream>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/detail/common_iarchive.hpp>
#include <numeric>
#include <future>



using namespace boost::archive;

void Index::AddFile(int file_id, const Meta &meta) {
    const auto &str = meta.GetPathById(file_id);
    std::ifstream in(str);

//    const auto &path = meta.GetPathById(file_id);
//    int fdin;
//    void *src;
//    struct stat statbuf;
//    if ((fdin = open(path.c_str(), O_RDONLY)) < 0) {
//        throw std::runtime_error("Can't open file " + path);
//    }
//    if ((fstat(fdin, &statbuf)) < 0) {
//        throw std::runtime_error("fstat error");
//    }
//    if ((src = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0)) == MAP_FAILED) {
//        throw std::runtime_error("Error in mmap file " + path);
//    }
//
//    if (statbuf.st_size < 3) {
//        // TODO all trigrams
//    }
//    char a = static_cast<char *>(src)[0];
//    char b = static_cast<char *>(src)[1];
//    for (int i = 2; i < statbuf.st_size; ++i) {
//        char c = static_cast<char *>(src)[i];
//        int tr = std::tolower(a) * 256 * 256 + std::tolower(b) * 256 + std::tolower(c);
//        index[tr].insert(file_id);
//        a = b;
//        b = c;
//    }
//    munmap(src, statbuf.st_size);
    char a, b, c;
    if (!in.get(a) || !in.get(b)) {
        // TODO all trigrams
    }
    while (in.get(c)) {
        int tr = std::tolower(a) * 256 * 256 + std::tolower(b) * 256 + std::tolower(c);
//        clock_t start = clock();
        index[tr].insert(file_id);
//        clock_t end = clock();
//        double seconds = (double)(end - start) / CLOCKS_PER_SEC;
//        if (str == "/home/nakap/diploma/ClickHouse/docs/en/interfaces/third-party/gui.md") {
//            std::cout << "Added trigram " << tr << " " << seconds << std::endl;
//        }
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
    IndexForSearch result;

    std::vector<std::pair<int, std::vector<int>>> pre;

    for (const auto &now: index) {
        pre.push_back({now.first, {}});


//        result.trigrams.push_back(now.first);
//        result.files_ids.push_back({});
//        int index = result.trigrams.size() - 1;

        int index = pre.size() - 1;

        for (const auto &file_id: now.second) {
            pre[index].second.push_back(file_id);
        }
        std::sort(pre[index].second.begin(), pre[index].second.end());

        //        for (const auto &file_id: now.second) {
//            result.files_ids[index].push_back(file_id);
//        }
//        std::sort(result.files_ids[index].begin(), result.files_ids[index].end());
    }
    std::sort(pre.begin(), pre.end(), [](auto &left, auto &right) {
        return left.first < right.first;
    });


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
//Message LoadIndex(const std::string &file_path) {
    clock_t st = clock();
    std::ifstream file{file_path};

//    Message m;
//    m.ParseFromIstream(&file);
//    std::vector<int> arr;
//    std::cout << "size = " << m2.id_size() << std::endl;
//    arr.reserve(m2.id().size());
//    arr.insert(arr.begin(), m2.id().begin(), m2.id().end());
//    for (auto it = m2.id().begin(); it != m2.id().end(); ++it) {
//        std::cout << *it << " :) ";
//    }

    Index::IndexForSearch index;
//    index.trigrams = {m.trigrams().begin(), m.trigrams().end()};
//
////    index.trigrams.reserve(m.trigrams().size());
////    for (const auto& a: m.trigrams()) {
////        index.trigrams.push_back(a);
////    }
////    int inde = 0;
//    for (const auto& files: m.files()) {
//        index.files_ids.push_back({files.file().begin(), files.file().end()});
////        index.files_ids.push_back({});
////        index.files_ids[inde].reserve(files.file().size());
////        for (const auto& b: files.file()) {
////            index.files_ids[inde].push_back(b);
////        }
////        inde++;
//    }

    clock_t end = clock();
    double seconds = (double) (end - st) / CLOCKS_PER_SEC;
//    std::cout << "Loaded proto " << m.trigrams().size() << " " << seconds << std::endl;
////    text_iarchive ia{file};
    binary_iarchive ia{file};
//    Index::IndexForSearch index;
    ia >> index;
//    clock_t end = clock();
//    double seconds = (double) (end - st) / CLOCKS_PER_SEC;
////    std::cout << "Load Index " << seconds << std::endl;
////    std::cout << index.trigrams.size() << "\n";



    return index;
}

void SaveIndex(const std::string &file_name, const Index &index) {
    auto index_for_save = index.ConvertForSave();
//    Message m;
//    for (const auto& tr: index_for_save.trigrams) {
//        m.add_trigrams(tr);
//    }
//    for (const auto& files: index_for_save.files_ids) {
//        Files* f = m.add_files();;
//        for (const auto& file: files) {
//            f->add_file(file);
//        }
//    }


    std::ofstream file{file_name};
//    m.SerializeToOstream(&file);


////    text_oarchive oa{file};
    binary_oarchive oa{file};
    oa << index_for_save;
}

void SaveIndexes(const std::vector<Index> &indexes) {
    for (size_t i = 0; i < indexes.size(); ++i) {
        SaveIndex(GetIndexFileName(i), indexes[i]);
    }
}

std::vector<Index::IndexForSearch> LoadIndexes(size_t cnt_indexes, const std::string &path) {
//std::vector<Message> LoadIndexes(size_t cnt_indexes, const std::string &path) {
        clock_t start = clock();
    std::vector<std::future<Index::IndexForSearch>> fut_index;
//    std::vector<std::future<Message>> fut_index;
    for (size_t i = 0; i < cnt_indexes; ++i) {
        auto file_path = path + GetIndexFileName(i);
        fut_index.push_back(std::async(std::launch::async, [file_path] { return LoadIndex(file_path); }));
    }
//    std::vector<Message> result;
    std::vector<Index::IndexForSearch> result;
    for (size_t i = 0; i < cnt_indexes; ++i) {
        result.push_back(fut_index[i].get());
    }
    clock_t end = clock();
    double seconds = (double) (end - start) / CLOCKS_PER_SEC;
//    std::cout << "Loaded Indexes " << seconds << std::endl;
    return result;
}