#include "search.h"
#include "regex.h"
#include "../models/shard.h"

#include <iostream>

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        std::cout << "no regex to search\n";
        return 0;
    }
    std::string regex = argv[1];
    RegexQuery query(regex);
    if (query.GetOperation() == RegexQuery::kNone) {
        return 0;
    }
    auto cnt_indexes = LoadCntIndexes();
    auto indexes = LoadIndexes(cnt_indexes);
    auto file_paths_by_id = LoadFilePathsById();
    auto result = Search(query, indexes, file_paths_by_id);

    int cnt = 0;

    for (const auto &shard: result) {
        cnt += shard.size();
    }
    std::cout << "Cnt = " << cnt << std::endl;
}
