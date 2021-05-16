#pragma once

#include "regex.h"
#include "../models/index.h"

#include <string>
#include <vector>

struct SearchResult {
    std::string file;
    long int offset;
};

std::vector<std::vector<SearchResult>>
Search(const RegexQuery &, const std::vector<Index::IndexForSearch> &, const std::vector<std::string> &);
