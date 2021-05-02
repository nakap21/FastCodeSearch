#pragma once

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/detail/common_iarchive.hpp>

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
    int value = 2; // TODO fix default value
private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &archive, const unsigned int version) {
        archive & BOOST_SERIALIZATION_NVP(description);
        archive & BOOST_SERIALIZATION_NVP(value);
    }
};
