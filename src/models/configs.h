#pragma once

#include "../../utils/serialization_unordered_set.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/detail/common_iarchive.hpp>
#include <boost/unordered_set.hpp>

using namespace  boost::unordered;

struct UpdateIntervalSec {
    std::string description = "Index update interval in seconds";
    int value = 5;
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
    int value = 200;
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
    int value = 15971520;
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
    unordered_set<std::string> value = {".tsv", ".gitignore", ".tsv", ".csv", ".reference", ".png",
                                             ".pdf", ".git", ".gif", ".svg"};
private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &archive, const unsigned int version) {
        archive & BOOST_SERIALIZATION_NVP(description);
        archive & BOOST_SERIALIZATION_NVP(value);
    }
};

struct CapFilesCnt {
    std::string description = "The capacity of search result";
    int value = 10000;
private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &archive, const unsigned int version) {
        archive & BOOST_SERIALIZATION_NVP(description);
        archive & BOOST_SERIALIZATION_NVP(value);
    }
};
