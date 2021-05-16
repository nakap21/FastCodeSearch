#pragma once

#include "configs.h"
#include "../utils/serialization_unordered_map.h"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/detail/common_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

using namespace boost::unordered;

class Meta {
public:
    Meta(const std::string &file_path = "meta_info.bin");

    void SaveMeta();

    void SetUpdateIntervalSec(int);

    void SetCntFilesInShard(int);

    void SetFilesFormatsIgnore(const unordered_set<std::string> &);

    void SetMaxSizeIndexFile(int);

    void SetCapFilesCnt(int);

    int AddFile(const std::string &);

    void DeleteFile(const std::string &);

    void StopEngine();

    void Clear();

    const UpdateIntervalSec &GetUpdateIntervalSec() const { return update_interval_sec; }

    const CntFilesInShard &GetCntFilesInShard() const { return cnt_files_in_shard; }

    const MaxSizeIndexFile &GetMaxSizeIndexFile() const { return max_size_index_file; }

    const FilesFormatsIgnore &GetFilesFormatsIgnore() const { return files_formats_ignore; }

    const CapFilesCnt &GetCapFilesCnt() const { return cap_files_cnt; }

    const unordered_map<int, time_t> &GetFiles() const { return files; }

    bool ShouldStopEngine() { return should_stop_engine; }

    const std::string &GetPathById(int ind) const { return file_paths[ind]; }

    const std::vector<std::string> &GetPaths() const { return file_paths; }

    void SaveFilePathsById();

    void SaveCapFilesCnt();

private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &archive, const unsigned int version) {
        archive & BOOST_SERIALIZATION_NVP(should_stop_engine);
        archive & BOOST_SERIALIZATION_NVP(update_interval_sec);
        archive & BOOST_SERIALIZATION_NVP(cnt_files_in_shard);
        archive & BOOST_SERIALIZATION_NVP(max_size_index_file);
        archive & BOOST_SERIALIZATION_NVP(files_formats_ignore);
        archive & BOOST_SERIALIZATION_NVP(cap_files_cnt);
        archive & BOOST_SERIALIZATION_NVP(files);
        archive & BOOST_SERIALIZATION_NVP(file_paths);
    }

    bool should_stop_engine = false;
    UpdateIntervalSec update_interval_sec;
    CntFilesInShard cnt_files_in_shard;
    MaxSizeIndexFile max_size_index_file;
    FilesFormatsIgnore files_formats_ignore;
    CapFilesCnt cap_files_cnt;
    unordered_map<int, time_t> files;
    std::vector<std::string> file_paths;
};
