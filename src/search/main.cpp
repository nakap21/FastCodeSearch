#include "search.h"
#include "regex.h"
#include "../models/shard.h"

#include <iostream>
#include <vector>
#include <future>
#include <time.h>

int main(int argc, char *argv[]) {
    Shards shards;
//    std::cout << "TUT-1\n";
    clock_t start = clock();
    shards.LoadShards();
    if (argc <= 1) {
        std::cout << "no regex to search\n";
        return 0;
    }
    clock_t end = clock();
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    std::cout << "Load Shards " << seconds << std::endl;
//    std::cout << "TUT0\n";
    std::string regex = argv[1];
    RegexQuery query(regex);
    if (query.GetOperation() == RegexQuery::kNone) {
        return 0;
    }
    std::cout << "TUT\n";
    auto indexes = shards.GetIndexData();
    std::cout << "TU2\n" << indexes.size();
    std::vector<std::future<std::vector<SearchResult>>> search_result;
    clock_t start1 = clock();
    Meta meta;
    clock_t end1 = clock();
    double seconds1 = (double)(end1 - start1) / CLOCKS_PER_SEC;
    std::cout << "Load Meta " << seconds1 << std::endl;
    clock_t start2 = clock();
    for (const auto &index: indexes) {
        search_result.push_back(std::async(std::launch::async, [&query, &index, &meta]{ return Search(query, index, meta); }));
    }
    for (auto &shard_result: search_result) {
        auto res = shard_result.get();
        for (const auto &result: res) {
//            std::cout << "file = " << result.file << ", offset = " << result.offset << std::endl;
        }
    }
    clock_t end2 = clock();
    double seconds2 = (double)(end2 - start2) / CLOCKS_PER_SEC;
    std::cout << "Get result " << seconds2 << std::endl;
}
