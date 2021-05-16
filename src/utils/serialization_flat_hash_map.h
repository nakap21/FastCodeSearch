
// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// serialization/map.hpp:
// serialization for stl map templates

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// See http://www.boost.org for updates, documentation, and revision history.

#include <absl/container/flat_hash_map.h>

#include <boost/config.hpp>

#include <boost/serialization/utility.hpp>
#include <boost/serialization/collections_save_imp.hpp>
#include <boost/serialization/collections_load_imp.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/map.hpp>


namespace boost {
    namespace serialization {

        template<class Archive, class Type, class Key, class Hash, class Compare, class Allocator >
        inline void save(
                Archive & ar,
                const absl::flat_hash_map<Key, Type, Hash, Compare, Allocator> &t,
                const unsigned int /* file_version */
        ){
            boost::serialization::stl::save_collection<
                    Archive,
                    absl::flat_hash_map<Key, Type, Hash, Compare, Allocator>
            >(ar, t);
        }

        template<class Archive, class Type, class Key, class Hash, class Compare, class Allocator >
        inline void load(
                Archive & ar,
                absl::flat_hash_map<Key, Type, Hash, Compare, Allocator> &t,
                const unsigned int /* file_version */
        ){
#if BOOST_VERSION >= 105800
            load_map_collection(ar, t);
#else
            boost::serialization::stl::load_collection<
        Archive, absl::flat_hash_map<Key, Type, Hash, Compare, Allocator>,
        boost::serialization::stl::archive_input_map<Archive, absl::flat_hash_map<Key, Type, Hash, Compare, Allocator>>,
        boost::serialization::stl::reserve_imp<boost::absl::flat_hash_map<Key, Type, Hash, Compare, Allocator>>>(ar, t);
#endif
        }

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
        template<class Archive, class Type, class Key, class Hash, class Compare, class Allocator >
        inline void serialize(
                Archive & ar,
                absl::flat_hash_map<Key, Type, Hash, Compare, Allocator> &t,
                const unsigned int file_version
        ){
            boost::serialization::split_free(ar, t, file_version);
        }

    } // serialization
} // namespace boost

