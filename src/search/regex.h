#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>


class RegexQuery {
public:
    enum RegexQueryOperation {
        kAll,
        kNone,
        kOr
    };

    RegexQuery(const std::string &);

    const std::string& GetRegex() const { return regex_for_match; }

    RegexQueryOperation GetOperation() const { return operation; }

    const std::vector<std::string> &GetSubs() const { return subs; }

private:

    std::string regex_for_match;
    RegexQueryOperation operation;
    std::vector<std::string> subs;
};
