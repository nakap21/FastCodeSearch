#pragma once

#include "index.h"
#include "meta.h"
#include "../utils/serialization_unordered_map.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/detail/common_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/unordered_map.hpp>

using namespace boost::unordered;

size_t LoadCntIndexes(const std::string& file_path = "cnt_indexes.bin");

class Shards {
public:
    void AddFile(int, const Meta &);

    void DeleteFile(int);

    void SaveShards();

    std::vector<Index::IndexForSearch> LoadShards();

    void Clear();

    const std::vector<Index>& GetIndexData() { return indexes; }

    const unordered_map<int, int> &GetFilesShard() { return files_shard; }

    const std::vector<int> &GetFilesCntOnShards() { return files_cnt_on_shrads; }


private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &archive, const unsigned int version) {
        archive & BOOST_SERIALIZATION_NVP(files_shard);
        archive & BOOST_SERIALIZATION_NVP(files_cnt_on_shrads);
    }

    void SaveCntIndexes();

    unordered_map<int, int> files_shard;
    std::vector<int> files_cnt_on_shrads;
    std::vector<Index> indexes;
};
