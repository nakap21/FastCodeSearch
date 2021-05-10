#include "configs.h"
#include "../models/meta.h"

#include <iostream>

void UpdateConfig(const std::string &config_name, const std::vector<std::string> &value) {
    Meta meta;
    if (config_name == "UPDATE_INTERVAL_SEC") {
        meta.SetUpdateIntervalSec(std::stoi(value[0]));
    } else if (config_name == "CNT_FILES_IN_SHARD") {
        meta.SetCntFilesInShard(std::stoi(value[0]));
    } else if (config_name == "MAX_SIZE_INDEX_FILE") {
        meta.SetMaxSizeIndexFile(std::stoi(value[0]));
    } else if (config_name == "FILES_FORMATS_IGNORE") {
        unordered_set<std::string> new_formats(value.begin(), value.end());
        meta.SetFilesFormatsIgnore(new_formats);
    } else {
        std::cout << "WARNING! There is no config with name " << config_name << std::endl;
    }
    meta.SaveMeta();
}

void ShowConfigsValue() {
    Meta meta;
    std::cout << "UPDATE_INTERVAL_SEC | " << meta.GetUpdateIntervalSec().description << " | "
              << meta.GetUpdateIntervalSec().value << std::endl;
    std::cout << "CNT_FILES_IN_SHARD | " << meta.GetCntFilesInShard().description << " | "
              << meta.GetCntFilesInShard().value << std::endl;
    std::cout << "MAX_SIZE_INDEX_FILE | " << meta.GetMaxSizeIndexFile().description << " | "
              << meta.GetMaxSizeIndexFile().value << std::endl;
    std::cout << "FILES_FORMATS_IGNORE | " << meta.GetFilesFormatsIgnore().description << " | { ";
    for (const auto &type: meta.GetFilesFormatsIgnore().value) {
        std::cout << "\"" << type << "\" ";
    }
    std::cout << "}" << std::endl;
}