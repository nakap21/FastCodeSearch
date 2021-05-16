#pragma once

#include "../models/meta.h"

#include <filesystem>

namespace fs = std::filesystem;

//void Delete(const std::string &file, Meta &meta);
//
//void Add(const std::string &file, Meta &meta);

void DeleteFiles(const std::vector<std::string> &files);

void AddFiles(const std::vector<std::string> &files);