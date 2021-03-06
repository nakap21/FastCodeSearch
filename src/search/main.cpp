#include "search.h"
#include "regex.h"
#include "../models/shard.h"

#include <iostream>

int main(int argc, char *argv[]) {
    try {
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
        auto cap_result = LoadCapFilesCnt();
        auto result = Search(query, indexes, file_paths_by_id);

        int cnt = 0;
        std::cout << "NAKAP " << cap_result << std::endl;
        for (const auto &shard: result) {
            for (const auto& shard_res: shard) {
                if (++cnt > cap_result) {
                    std::cout << "Search result is too large. It is truncated. Current result's count = " << cap_result << std::endl;
                    return 0;
                }
                std::cout << "file = " << shard_res.file << ", offset = " << shard_res.offset << std::endl;
            }
        }
        std::cout << "Search result count = " << cnt << std::endl;
    } catch (const std::exception &ex) {
        std::cout << ex.what() << std::endl;
    }
}
