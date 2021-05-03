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

std::vector<SearchResult> Search(const RegexQuery &, const Index &, const Meta&);
