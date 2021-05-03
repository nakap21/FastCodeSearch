#pragma once

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/detail/common_iarchive.hpp>
#include <boost/serialization/unordered_set.hpp>

struct UpdateIntervalSec {
    std::string description = "Index update interval in seconds";
    int value = 5; // TODO fix default value
private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &archive, const unsigned int version) {
        archive & BOOST_SERIALIZATION_NVP(description);
        archive & BOOST_SERIALIZATION_NVP(value);
    }
};

struct CntFilesInShard {
    std::string description = "Count of files per shard";
    int value = 30; // TODO fix default value
private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &archive, const unsigned int version) {
        archive & BOOST_SERIALIZATION_NVP(description);
        archive & BOOST_SERIALIZATION_NVP(value);
    }
};

struct MaxSizeIndexFile {
    std::string description = "Max size files in index";
    int value = 20971520; // TODO fix default value
private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &archive, const unsigned int version) {
        archive & BOOST_SERIALIZATION_NVP(description);
        archive & BOOST_SERIALIZATION_NVP(value);
    }
};

struct FilesFormatsIgnore {
    std::string description = "Max size files in index";
    std::unordered_set<std::string> value = {".tsv", ".gitignore", ".tsv", ".csv", ".reference", ".png",
                                             ".pdf", ".git", ".gif", ".svg"}; // TODO fix default value
private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &archive, const unsigned int version) {
        archive & BOOST_SERIALIZATION_NVP(description);
        archive & BOOST_SERIALIZATION_NVP(value);
    }
};
