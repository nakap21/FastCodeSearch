#pragma once

#include "regex.h"
#include "../models/index.h"

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

struct SearchResult {
    std::string file;
    long int offset;
};

std::vector<std::vector<SearchResult>>
Search(const RegexQuery &, const std::vector<Index::IndexForSearch> &, const std::vector<std::string> &);
