#pragma once

#include "meta.h"
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>
//#include <boost/archive/detail/common_iarchive.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include "../utils/serialization_unordered_map.h"
#include "../utils/serialization_unordered_set.h"


#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
//using namespace  boost::container;
using namespace boost::unordered;
class Index {
public:
    void AddFile(int, const Meta&);

    void DeleteFile(int);

    const unordered_map<int, unordered_set<int>> &GetIndex() const { return index; }

private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &archive, const unsigned int version) {
        archive & BOOST_SERIALIZATION_NVP(index);
    }

    unordered_map<int, unordered_set<int>> index;
};
