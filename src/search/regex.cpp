#include "regex.h"

#include <re2/re2.h>
#include <re2/prefilter_tree.h>
#include <re2/prefilter.h>
#include <iostream>

using namespace re2;

namespace {
    bool IsSmallRegex(const std::string &regex) {
        return regex.size() < 3;
    }
}

RegexQuery::RegexQuery(const std::string &regex) {
    if (IsSmallRegex(regex)) {
        operation = kAll;
        return;
    }
    regex_for_match = "(" + regex + ")";
    RE2 re(regex_for_match);
    Prefilter *pf = re2::Prefilter::FromRE2(&re);
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
    tree.Add(pf);
    tree.Compile(&subs);
    operation = kOr;
    if (subs.empty()) {
        operation = kAll;
    }
}
