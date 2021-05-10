#include "search.h"

#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>
#include <boost/container/flat_set.hpp>
#include <fcntl.h>
#include <future>
#include <iostream>
#include <re2/re2.h>
#include <re2/stringpiece.h>
#include <sys/stat.h>
#include <sys/mman.h>

using namespace re2;

namespace {

    absl::flat_hash_map<int, int>
    GetTrigramMap(const Index::IndexForSearch &index, const absl::flat_hash_set<int> &all_trigrams) {
        absl::flat_hash_map<int, int> result;
        for (const auto &tr: all_trigrams) {
            auto upper = std::upper_bound(index.trigrams.begin(), index.trigrams.end(), tr);
            if (upper == index.trigrams.begin()) { ;
            } else {
                upper--;
                if (*upper == tr) {
                    result.insert({tr, upper - index.trigrams.begin()});
                }
            }
        }
        return result;
    }

    std::vector<int>
    GetFilesWithTr(int tr, const Index::IndexForSearch &index, const absl::flat_hash_map<int, int> &trigram_map) {
        auto it = trigram_map.find(tr);
        if (it == trigram_map.end()) {
            return {};
        }
        return index.files_ids[it->second];
    }

    std::vector<int> AndTwoVectors(const std::vector<int> &a, const std::vector<int> &b) {
        std::vector<int> res;
        int Ia = 0;
        int Ib = 0;
        while (Ia < a.size() && Ib < b.size()) {
            if (a[Ia] < b[Ib]) {
                Ia++;
            } else if (a[Ia] > b[Ib]) {
                Ib++;
            } else {
                res.push_back(a[Ia]);
                Ia++;
                Ib++;
            }
        }
        return res;
    }

    std::vector<int> OrTwoVectors(const std::vector<int> &a, const std::vector<int> &b) {
        std::vector<int> res;
        int Ia = 0;
        int Ib = 0;
        while (Ia < a.size() || Ib < b.size()) {
            if (Ia == a.size()) {
                res.push_back(b[Ib]);
                Ib++;
                continue;
            }
            if (Ib == b.size()) {
                res.push_back(a[Ia]);
                Ia++;
                continue;
            }
            if (a[Ia] < b[Ib]) {
                res.push_back(a[Ia]);
                Ia++;
                continue;
            } else if (a[Ia] > b[Ib]) {
                res.push_back(b[Ib]);
                Ib++;
                continue;
            } else {
                res.push_back(a[Ia]);
                Ia++;
                Ib++;
            }
        }
        return res;
    }

    std::vector<int> AndTrigrams(const std::vector<int> &trigrams,
                                 const Index::IndexForSearch &index,
                                 const absl::flat_hash_map<int, int> &trigram_map) {
        if (trigrams.empty()) {
            return {};
        }
        auto result = GetFilesWithTr(trigrams[0], index, trigram_map);

        for (size_t i = 1; i < trigrams.size(); ++i) {
            std::vector<int> to_del;
            for (const auto &cur: result) {
                auto cur_files = GetFilesWithTr(trigrams[i], index, trigram_map);
                auto and_res = AndTwoVectors(result, cur_files);
                result = std::move(and_res);
            }
        }
        return result;
    }

    std::vector<int> GetAllFiles(const Index::IndexForSearch &index) {
        if (index.files_ids.empty()) {
            return {};
        }
        std::vector<int> files = index.files_ids[0];
        for (int i = 1; i < index.files_ids.size(); ++i) {
            files = OrTwoVectors(files, index.files_ids[i]);
        }
        return files;
    }

    std::vector<int> GetTrigrams(const std::string &regex) {
        std::vector<int> trigrams;
        char a, b, c;
        a = regex[0];
        b = regex[1];
        for (size_t i = 2; i < regex.size(); ++i) {
            c = regex[i];
            int tr = std::tolower(a) * 256 * 256 + std::tolower(b) * 256 + std::tolower(c);
            trigrams.push_back(tr);
            a = b;
            b = c;
        }
        return trigrams;
    }

    std::vector<int>
    GetFilesToSearch(const RegexQuery &query, const Index::IndexForSearch &index,
                     const absl::flat_hash_map<int, int> &trigram_map) {
        if (query.GetOperation() == RegexQuery::kAll) {
            return GetAllFiles(index);
        }

        std::vector<int> files;
        for (const auto &regex: query.GetSubs()) {
            if (regex.size() < 3) {
                return GetAllFiles(index);
            }
            auto trigrams = GetTrigrams(regex);
            auto files_for_trigrams = AndTrigrams(trigrams, index, trigram_map);
            files = OrTwoVectors(files, files_for_trigrams);
        }
        return files;
    }

}

absl::flat_hash_set<int> GetAllTrigrams(const RegexQuery &query) {
    absl::flat_hash_set<int> result;
    for (const auto &sub: query.GetSubs()) {
        for (const auto &tr: GetTrigrams(sub)) {
            result.insert(tr);
        }
    }
    return result;
}


std::vector<SearchResult>
SearchInFile(int path_id, const RegexQuery &query, const std::vector<std::string> &file_paths_by_id) {
    std::vector<SearchResult> search_result;
    const auto &path = file_paths_by_id[path_id];
    int fdin;
    void *src;
    struct stat statbuf;
    if ((fdin = open(path.c_str(), O_RDONLY)) < 0) {
        throw std::runtime_error("Can't open file " + path);
    }
    if ((fstat(fdin, &statbuf)) < 0) {
        close(fdin);
        throw std::runtime_error("fstat error");
    }
    if ((src = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0)) == MAP_FAILED) {
        close(fdin);
        throw std::runtime_error("Error in mmap file " + path);
    }
    StringPiece file(static_cast<const char *>(src), statbuf.st_size);
    StringPiece file_begin(static_cast<const char *>(src), statbuf.st_size);
    StringPiece result;
    RE2 for_search(query.GetRegex());
    while (RE2::FindAndConsume(&file, for_search, &result)) {
        search_result.push_back({path, result.data() - file_begin.data()});
    }
    munmap(src, statbuf.st_size);
    close(fdin);
    return search_result;
}

std::vector<SearchResult>
SearchInIndex(const RegexQuery &query, const Index::IndexForSearch &index,
              const std::vector<std::string> &file_paths_by_id) {
    std::vector<SearchResult> search_result;
    auto all_trigrams = GetAllTrigrams(query);
    auto trigram_map = GetTrigramMap(index, all_trigrams);
    auto files_to_search = GetFilesToSearch(query, index, trigram_map);
    std::vector<std::future<std::vector<SearchResult>>> file_search_future_result;
    for (const auto &path_id: files_to_search) {
        auto search_in_file = SearchInFile(path_id, query, file_paths_by_id);
        for (const auto &search_res: search_in_file) {
            search_result.push_back(search_res);
        }
    }
    for (auto &fut_search_res: file_search_future_result) {
        auto file_res = fut_search_res.get();
        for (const auto &add: file_res) {
            search_result.push_back(add);
        }
    }
    return search_result;
}

std::vector<std::vector<SearchResult>>
Search(const RegexQuery &query, const std::vector<Index::IndexForSearch> &indexes,
       const std::vector<std::string> &file_paths_by_id) {
    std::vector<std::future<std::vector<SearchResult>>> search_future_result;
    std::vector<std::vector<SearchResult>> search_result;
    for (const auto &index: indexes) {
        search_future_result.push_back(
                std::async(std::launch::async, [&query, &index, &file_paths_by_id] {
                    return SearchInIndex(query, index, file_paths_by_id);
                }));
    }
    for (auto &shard_result: search_future_result) {
        search_result.push_back(shard_result.get());
    }
    return search_result;
}

