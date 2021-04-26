#include "index.h"

#include <fstream>

void Index::AddFile(const std::string &path) {
    std::ifstream in(path);
    // TODO check file length
    char a, b, c;
    in.get(a);
    in.get(b);
    while (in.get(c)) {
        int tr = a * 256 * 256 + b * 256 + c;
        index[tr].insert(path);
        a = b;
        b = c;
    }
    in.close();
}

void Index::DeleteFile(const std::string &path) {
    for (auto &now: index) {
        now.second.erase(path);
    }
}
