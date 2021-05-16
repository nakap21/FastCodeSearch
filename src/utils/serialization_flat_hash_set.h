#pragma once


#include <boost/serialization/set.hpp>
#include <absl/container/flat_hash_set.h>

namespace boost {
    namespace serialization {

        template <class Archive, class... Types>
        inline void save(Archive& ar, const absl::flat_hash_set<Types...>& t, const unsigned int /* file_version */
        ) {
            boost::serialization::stl::save_collection<Archive, absl::flat_hash_set<Types...>>(ar, t);
        }

        template <class Archive, class... Types>
        inline void load(Archive& ar, absl::flat_hash_set<Types...>& t, const unsigned int /* file_version */
        ) {
#if BOOST_VERSION >= 105800
            load_set_collection(ar, t);
#else
            boost::serialization::stl::load_collection<
        Archive, boost::container::flat_set<Types...>,
        boost::serialization::stl::archive_input_set<Archive, boost::container::flat_set<Types...>>,
        boost::serialization::stl::reserve_imp<boost::container::flat_set<Types...>>>(ar, t);
#endif
        }

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
        template <class Archive, class... Types>
        inline void serialize(Archive& ar, absl::flat_hash_set<Types...>& t, const unsigned int file_version) {
            boost::serialization::split_free(ar, t, file_version);
        }

    }  // namespace serialization
}  // namespace boost