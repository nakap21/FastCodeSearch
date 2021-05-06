#include "regex.h"

#include <re2/re2.h>
#include <re2/prefilter_tree.h>
#include <re2/prefilter.h>
#include <iostream>
#include <memory>

using namespace re2;

namespace {
    bool IsSmallRegex(const std::string &regex) {
        return regex.size() < 3;
    }
}

RegexQuery::RegexQuery(const std::string &regex) {
    regex_for_match = "(" + regex + ")";
    if (IsSmallRegex(regex)) {
        operation = kAll;
        return;
    }
    std::shared_ptr<RE2> re = std::make_shared<RE2>(regex_for_match);
    std::unique_ptr<Prefilter> pf(re2::Prefilter::FromRE2(re.get()));
    if (pf == NULL) {
        std::cout << "ERROR! Wrong regular expression!\n";
        operation = kNone;
        return;
    }
    auto op = pf->op();
    if (op == Prefilter::ALL) {
        operation = kAll;
        return;
    }
    if (op == Prefilter::NONE) {
        operation = kNone;
        return;
    }
    if (op == Prefilter::ATOM) {
        if (IsSmallRegex(pf->atom())) {
            operation = kAll;
            return;
        }
        operation = kOr;
        subs.push_back(pf->atom());
        return;
    }
    PrefilterTree tree;
    tree.Add(pf.get());
    tree.Compile(&subs);
    operation = kOr;
    if (subs.empty()) {
        operation = kAll;
    }
}
