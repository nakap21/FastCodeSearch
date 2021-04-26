#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

struct SearchResult {
    std::string file;
    long int offset;
};

enum QueryOperation {
    kAnd,
    kOr
};

class Query {
public:
    Query(const std::string &);

    std::unordered_set<std::string> ExecuteQuery(const std::unordered_map<int, std::unordered_set<std::string>> &);

private:
    Query(QueryOperation operation, std::vector<int> trigrams, Query *sub)
            : operation(operation),
              trigrams(std::move(trigrams)),
              sub(std::move(sub)) {}

    QueryOperation operation;
    std::vector<int> trigrams;
    Query *sub = nullptr;
};

std::vector<SearchResult> Search(const std::string &, const std::unordered_map<int, std::unordered_set<std::string>> &);
