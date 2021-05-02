#pragma once

#include "regex.h"

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

struct SearchResult {
    std::string file;
    long int offset;
};

std::vector<SearchResult> Search(const RegexQuery&, const std::unordered_map<int, std::unordered_set<std::string>> &);
