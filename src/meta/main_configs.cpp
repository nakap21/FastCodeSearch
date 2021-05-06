#include "../models/meta.h"

#include <iostream>
#include <vector>

namespace {
    void UpdateConfig(const std::string &config_name, const std::vector<std::string> &value) {
        Meta meta;
        if (config_name == "UPDATE_INTERVAL_SEC") {
            meta.SetUpdateIntervalSec(std::stoi(value[0]));
        } else if (config_name == "CNT_FILES_IN_SHARD") {
            meta.SetCntFilesInShard(std::stoi(value[0]));
        } else if (config_name == "MAX_SIZE_INDEX_FILE") {
            meta.SetMaxSizeIndexFile(std::stoi(value[0]));
        } else if (config_name == "FILES_FORMATS_IGNORE") {
            std::unordered_set<std::string> new_formats(value.begin(), value.end());
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
        for (const auto& type: meta.GetFilesFormatsIgnore().value) {
            std::cout << "\"" << type << "\" ";
        }
        std::cout << "}" << std::endl;
    }
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        std::cout << "Wrong format! Template of this command is 'show' or 'update *config_name* *value*'\n";
        return 0;
    }
    std::string action = argv[1];
    if (action != "update" && action != "show") {
        std::cout
                << "Wrong format! Second arg should be 'show' or 'update'. Template of this command is 'show' or 'update *config_name* *value*'\n";
        return 0;
    }
    if (action == "update" && argc < 4) {
        std::cout << "Wrong format! For update template is 'update *config_name* *value*'\n";
        return 0;
    }
//    TODO add update new configs
    if (action == "update") {
        std::string config_name = argv[2];
        std::vector<std::string> value;
        for (int i = 3; i < argc; ++i) {
            value.push_back(argv[i]);
        }
        UpdateConfig(config_name, value);
    } else {
        ShowConfigsValue();
    }
}
