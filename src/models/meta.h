#include "configs.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/detail/common_iarchive.hpp>
#include <boost/serialization/unordered_map.hpp>

class Meta {
public:
    Meta();

    void SaveMeta();

    void SetUpdateIntervalSec(int);

    void SetCntFilesInShard(int);

    void AddFile(const std::string &);

    void DeleteFile(const std::string &);

    const UpdateIntervalSec &GetUpdateIntervalSec() const { return UPDATE_INTERVAL_SEC; }

    const CntFilesInShard &GetCntFilesInShard() const { return CNT_FILES_IN_SHARD; }

    const std::unordered_map<std::string, time_t> &GetFiles() const { return files; }

    bool ShouldStopEngine() { return false; }

private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &archive, const unsigned int version) {
        archive & BOOST_SERIALIZATION_NVP(UPDATE_INTERVAL_SEC);
        archive & BOOST_SERIALIZATION_NVP(CNT_FILES_IN_SHARD);
        archive & BOOST_SERIALIZATION_NVP(files);
    }

    UpdateIntervalSec UPDATE_INTERVAL_SEC;
    CntFilesInShard CNT_FILES_IN_SHARD;
    std::unordered_map<std::string, time_t> files;
};
