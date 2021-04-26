#include "meta.h"

#include <fstream>
#include <iostream>
#include <sys/stat.h>

using namespace boost::archive;

Meta::Meta() {
    std::ifstream file{"meta_info.txt"};
    try {
        text_iarchive ia{file};
        ia >> *this;
    } catch (const std::exception &ex) {
        std::cout << "No meta file " << ex.what() << "\n";
    }
}

void Meta::SaveMeta() {
    std::ofstream file{"meta_info.txt"};
    text_oarchive oa{file};
    oa << *this;
}

void Meta::AddFile(const std::string &file) {
    struct stat fileInfo;
    stat(file.c_str(), &fileInfo);
    files[file] = fileInfo.st_mtim.tv_sec;
}

void Meta::DeleteFile(const std::string &file) {
    files.erase(file);
}

void Meta::SetUpdateIntervalSec(int new_value) {
    UPDATE_INTERVAL_SEC.value = new_value;
}

void Meta::SetCntFilesInShard(int new_value) {
    CNT_FILES_IN_SHARD.value = new_value;
}