#include "search.h"
#include "regex.h"
#include "../models/shard.h"

#include <iostream>
#include <vector>

int main(int argc, char *argv[]) {
    Shards shards;
    shards.LoadShards();
    if (argc <= 1) {
        std::cout << "no regex to search\n";
        return 0;
    }
    std::string regex = argv[1];
    RegexQuery query(regex);
    if (query.GetOperation() == RegexQuery::kNone) {
        return 0;
    }

    auto indexes = shards.GetIndexData();
    std::vector<std::vector<SearchResult>> search_result;
    for (const auto &index: indexes) {
        search_result.push_back(Search(query, index.GetIndex()));
    }
    for (const auto &shard_result: search_result) {
        for (const auto &result: shard_result) {
            std::cout << "file = " << result.file << ", offset = " << result.offset << std::endl;
        }
    }
}
