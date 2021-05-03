#include "index.h"

#include <fstream>
#include <optional>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <time.h>

#include <iostream>

void Index::AddFile(int file_id, const Meta& meta) {
    const auto& str = meta.GetPathById(file_id);
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
