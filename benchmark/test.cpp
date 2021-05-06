#include <benchmark/benchmark.h>
#include "../src/search/search.h"
#include "../src/search/regex.h"
#include "../src/models/meta.h"
#include "../src/models/index.h"
#include "../src/models/shard.h"

#include <vector>

static void BM_SomeFunction(benchmark::State &state) {
    // Perform setup here

    for (auto _ : state) {
        // This code gets timed
        RegexQuery query("integ");
        auto file_paths_by_id = LoadFilePathsById("static/files_path_by_id.bin");
        auto cnt_indexes = LoadCntIndexes("static/cnt_indexes.bin");

        std::vector<Index::IndexForSearch> indexes = LoadIndexes(cnt_indexes,
                                                                 "static/");
        Search(query, indexes, file_paths_by_id);
    }
}
// Register the function as a benchmark
BENCHMARK(BM_SomeFunction);
// Run the benchmark
BENCHMARK_MAIN();