#include "search.h"

#include <fcntl.h>
#include <re2/re2.h>
#include <re2/stringpiece.h>
#include <sys/stat.h>
#include <sys/mman.h>

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

    std::unordered_set<std::string> OrTrigrams(const std::vector<int> &trigrams,
                                               const std::unordered_map<int, std::unordered_set<std::string>> &index) {
        if (trigrams.empty()) {
            return {};
        }
        std::unordered_set<std::string> result = GetFilesWithTr(trigrams[0], index);
        for (size_t i = 1; i < trigrams.size(); ++i) {
            for (const auto &cur: result) {
                auto cur_files = GetFilesWithTr(trigrams[i], index);
                for (const auto &to_add: cur_files) {
                    result.insert(to_add);
                }
            }
        }
    }

}

Query::Query(const std::string &regex) {
//    TODO library for parsing regex
//    TODO add kAll for size < 3 and other cases
    operation = kAnd;
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

}

std::unordered_set<std::string>
Query::ExecuteQuery(const std::unordered_map<int, std::unordered_set<std::string>> &index) {
    std::unordered_set<std::string> result;
    if (operation == kAnd) {
        result = AndTrigrams(trigrams, index);
    } else {
        result = OrTrigrams(trigrams, index);
    }

    if (sub != nullptr) {
//        TODO
    }

    return result;
}

std::vector<SearchResult>
Search(const std::string &regex, const std::unordered_map<int, std::unordered_set<std::string>> &index) {
    std::vector<SearchResult> search_result;
    Query regex_query(regex);
    auto files_to_search = regex_query.ExecuteQuery(index);

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
        std::string regex_for_match = "(" + regex + ")";
        re2::StringPiece result;
        while (re2::RE2::FindAndConsume(&file, regex_for_match, &result)) {
            search_result.push_back({path, result.data() - file_begin.data()});
        }
    }
    return search_result;
}
