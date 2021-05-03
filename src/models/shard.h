#pragma once

#include "index.h"
#include "meta.h"
#include "../utils/serialization_unordered_map.h"

//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/detail/common_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <unordered_map>

#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>

#include <boost/unordered_map.hpp>
//using namespace  boost::container;
using namespace boost::unordered;

class Shards {
public:
    void AddFile(int, const Meta &);

    void DeleteFile(int);

    void SaveShards();

    void LoadShards();

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

    unordered_map<int, int> files_shard;
    std::vector<int> files_cnt_on_shrads;
    std::vector<Index> indexes;
};
