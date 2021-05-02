#include "search.h"

#include <fcntl.h>
#include <re2/re2.h>
#include <re2/stringpiece.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <iostream>

using namespace re2;

namespace {

    std::unordered_set<std::string>
    GetFilesWithTr(int tr, const std::unordered_map<int, std::unordered_set<std::string>> &index) {
        auto it = index.find(tr);
        if (it == index.end()) {
            return {};
        }
        return it->second;
    }

    std::unordered_set<std::string> AndTrigrams(const std::vector<int> &trigrams,
                                                const std::unordered_map<int, std::unordered_set<std::string>> &index) {
        if (trigrams.empty()) {
            return {};
        }
        std::unordered_set<std::string> result = GetFilesWithTr(trigrams[0], index);

        for (size_t i = 1; i < trigrams.size(); ++i) {
            std::vector<std::string> to_del;
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

    std::unordered_set<std::string> GetAllFiles(const std::unordered_map<int, std::unordered_set<std::string>> &index) {
        std::unordered_set<std::string> files;
        for (const auto& tr: index) {
            for (const auto& file: tr.second) {
                files.insert(file);
            }
        }
        return files;
    }

    std::vector<int> GetTrigrams(const std::string& regex) {
        std::vector<int> trigrams;
        char a, b, c;
        a = regex[0];
        b = regex[1];
        for (size_t i = 2; i < regex.size(); ++i) {
            c = regex[i];
            int tr = a * 256 * 256 + b * 256 + c;
            trigrams.push_back(tr);
            a = b;
            b = c;
        }
        return trigrams;
    }

    std::unordered_set<std::string> GetFilesToSearch(const RegexQuery& query, const std::unordered_map<int, std::unordered_set<std::string>> &index) {
        if (query.GetOperation() == RegexQuery::kAll) {
            return GetAllFiles(index);
        }

        std::unordered_set<std::string> files;
        for (const auto& regex: query.GetSubs()) {
            if (regex.size() < 3) {
                return GetAllFiles(index);
            }
            auto trigrams = GetTrigrams(regex);
            auto files_for_trigrams = AndTrigrams(trigrams, index);
            for (const auto& new_file: files_for_trigrams) {
                files.insert(new_file);
            }
        }
        return files;
    }

}


std::vector<SearchResult>
Search(const RegexQuery& query, const std::unordered_map<int, std::unordered_set<std::string>> &index) {
    std::vector<SearchResult> search_result;
    auto files_to_search = GetFilesToSearch(query, index);
    for (const auto &path: files_to_search) {
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
    }
    return search_result;
}
