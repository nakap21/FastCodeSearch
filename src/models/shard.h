#include "index.h"
#include "meta.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/detail/common_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <unordered_map>

class Shards {
public:
    void AddFile(const std::string &, const Meta &);

    void DeleteFile(const std::string &);

    void SaveShards();

    void LoadShards();

    std::vector<Index> GetIndexData();

    const std::unordered_map<std::string, int> &GetFilesShard() { return files_shard; }

    const std::vector<int> &GetFilesCntOnShards() { return files_cnt_on_shrads; }


private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &archive, const unsigned int version) {
        archive & BOOST_SERIALIZATION_NVP(files_shard);
        archive & BOOST_SERIALIZATION_NVP(files_cnt_on_shrads);
    }

    std::unordered_map<std::string, int> files_shard;
    std::vector<int> files_cnt_on_shrads;
};
