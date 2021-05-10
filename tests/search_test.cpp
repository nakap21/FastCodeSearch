#include "gtest/gtest.h"
#include "../src/search/search.h"

#include "../src/models/meta.h"
#include "../src/models/index.h"
#include "../src/models/shard.h"

struct ExpSearchRes {
    std::string in;
    int expect_res;
};

static std::vector<ExpSearchRes> params = {{"test",     3},
                                    {"nakap",    0},
                                    {"(t|T)est", 5}};

TEST(search_test_case, search_test) {
    auto file_paths_by_id = LoadFilePathsById("static/files_path_by_id.bin");
    auto cnt_indexes = LoadCntIndexes("static/cnt_indexes.bin");
    EXPECT_EQ(cnt_indexes, 1);

    std::vector<Index::IndexForSearch> indexes = LoadIndexes(cnt_indexes,
                                                             "static/");


    for (const auto param: params) {
        RegexQuery query(param.in);
        auto search_result = Search(query, indexes, file_paths_by_id);
        EXPECT_EQ(search_result.size(), 1);
        EXPECT_EQ(search_result[0].size(), param.expect_res);
    }

}
