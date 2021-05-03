#include "search.h"

#include <fcntl.h>
#include <re2/re2.h>
#include <re2/stringpiece.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <iostream>

#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
//using namespace  boost::container;
using namespace boost::unordered;
using namespace re2;


namespace {

    unordered_set<int>
    GetFilesWithTr(int tr, const unordered_map<int, unordered_set<int>> &index) {
        auto it = index.find(tr);
        if (it == index.end()) {
            return {};
        }
        return it->second;
    }

    unordered_set<int> AndTrigrams(const std::vector<int> &trigrams,
                              const unordered_map<int, unordered_set<int>> &index) {
        if (trigrams.empty()) {
            return {};
        }
        auto result = GetFilesWithTr(trigrams[0], index);

        for (size_t i = 1; i < trigrams.size(); ++i) {
            std::vector<int> to_del;
            for (const auto &cur: result) {
                auto cur_files = GetFilesWithTr(trigrams[i], index);
                if (cur_files.find(cur) == cur_files.end()) {
                    to_del.push_back(cur);
                }
            }
            for (const auto &del: to_del) {
                result.erase(del);
            }
        }
        return result;
    }

    unordered_set<int> GetAllFiles(const unordered_map<int, unordered_set<int>> &index) {
    unordered_set<int> files;
        for (const auto &tr: index) {
            for (const auto &file: tr.second) {
                files.insert(file);
            }
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

    unordered_set<int> GetFilesToSearch(const RegexQuery &query, const unordered_map<int, unordered_set<int>> &index) {
        if (query.GetOperation() == RegexQuery::kAll) {
            return GetAllFiles(index);
        }

        unordered_set<int> files;
        for (const auto &regex: query.GetSubs()) {
            if (regex.size() < 3) {
                return GetAllFiles(index);
            }
            auto trigrams = GetTrigrams(regex);
            auto files_for_trigrams = AndTrigrams(trigrams, index);
            for (const auto &new_file: files_for_trigrams) {
                files.insert(new_file);
            }
        }
        return files;
    }

}


std::vector<SearchResult>
Search(const RegexQuery &query, const Index &index, const Meta &meta) {
    std::vector<SearchResult> search_result;
    auto files_to_search = GetFilesToSearch(query, index.GetIndex());
    for (const auto &path_id: files_to_search) {
        const auto &path = meta.GetPathById(path_id);
        int fdin;
        void *src;
        struct stat statbuf;
        if ((fdin = open(path.c_str(), O_RDONLY)) < 0) {
            throw std::runtime_error("Can't open file " + path);
        }
        if ((fstat(fdin, &statbuf)) < 0) {
            throw std::runtime_error("fstat error");
        }
        if ((src = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0)) == MAP_FAILED) {
            throw std::runtime_error("Error in mmap file " + path);
        }

        StringPiece file(static_cast<char *>(src));
        StringPiece file_begin(static_cast<char *>(src));
        re2::StringPiece result;
        while (re2::RE2::FindAndConsume(&file, query.GetRegex(), &result)) {
            search_result.push_back({path, result.data() - file_begin.data()});
        }
        munmap(src, statbuf.st_size);
    }
    return search_result;
}
