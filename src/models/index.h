#pragma once

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/detail/common_iarchive.hpp>
#include <boost/serialization/unordered_set.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <unordered_map>
#include <unordered_set>

class Index {
public:
    void AddFile(const std::string &);

    void DeleteFile(const std::string &);

    const std::unordered_map<int, std::unordered_set<std::string>> &GetIndex() const { return index; }

private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &archive, const unsigned int version) {
        archive & BOOST_SERIALIZATION_NVP(index);
    }

    std::unordered_map<int, std::unordered_set<std::string>> index;
};
