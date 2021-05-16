#pragma once

#include "meta.h"

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

using namespace boost::unordered;

class Index {
public:
    struct IndexForSearch {
        std::vector<int> trigrams;
        std::vector<std::vector<int>> files_ids;
    private:
        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive &archive, const unsigned int version) {
            archive & BOOST_SERIALIZATION_NVP(trigrams);
            archive & BOOST_SERIALIZATION_NVP(files_ids);
        }
    };

    void AddFile(int, const Meta &);

    void DeleteFile(int);

    IndexForSearch ConvertForSave() const;

private:
    unordered_map<int, unordered_set<int>> index;
};

void SaveIndexes(const std::vector<Index> &);

std::vector<Index::IndexForSearch> LoadIndexes(size_t, const std::string &path = "./");

std::string GetIndexFileName(int index);

std::vector<std::string> LoadFilePathsById(const std::string &file_path = "files_path_by_id.bin");

void DeleteIndex(const std::string& file_path);
