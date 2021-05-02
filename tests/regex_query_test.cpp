#include "gtest/gtest.h"
#include "../src/search/regex.h"

struct QueryTest {
    std::string in;
    RegexQuery::RegexQueryOperation expect_op;
    std::unordered_set<std::string> expect_subs;
};

std::vector<QueryTest> params = {
        {"Abcdef", RegexQuery::kOr, {"abcdef"}},
        {"b", RegexQuery::kAll, {}},
        {"abc(def|ghi)", RegexQuery::kOr, {"abcdef", "abcghi"}},
};

TEST(regex_query_test_case, regex_query_test) {
    for (const auto& param: params) {
        RegexQuery now_query(param.in);
        EXPECT_EQ(now_query.GetOperation(), param.expect_op);
        EXPECT_EQ(now_query.GetSubs().size(), param.expect_subs.size());
        for (const auto& sub: now_query.GetSubs()) {
            EXPECT_NE(param.expect_subs.end(), param.expect_subs.find(sub));
        }
    }
}