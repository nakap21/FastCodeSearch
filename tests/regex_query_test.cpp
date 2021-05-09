#include "gtest/gtest.h"
#include "../src/search/regex.h"

#include <unordered_set>

struct QueryTest {
    std::string in;
    RegexQuery::RegexQueryOperation expect_op;
    std::unordered_set<std::string> expect_subs;
    std::string exp_regex_for_match;
};

std::vector<QueryTest> params = {
        {"Abcdef",       RegexQuery::kOr,  std::unordered_set<std::string>{"abcdef"},           "(Abcdef)"},
        {"b",            RegexQuery::kAll, std::unordered_set<std::string>{},                   "(b)"},
        {"abc(def|ghi)", RegexQuery::kOr,  std::unordered_set<std::string>{"abcdef", "abcghi"}, "(abc(def|ghi))"},
};


TEST(regex_query_test_case, regex_query_test) {
    for (const auto &param: params) {
        RegexQuery now_query(param.in);
        EXPECT_EQ(now_query.GetOperation(), param.expect_op);
        EXPECT_EQ(now_query.GetSubs().size(), param.expect_subs.size());
        for (const auto &sub: now_query.GetSubs()) {
            EXPECT_NE(param.expect_subs.end(), param.expect_subs.find(sub));
        }
        EXPECT_EQ(now_query.GetRegex(), param.exp_regex_for_match);
    }
}