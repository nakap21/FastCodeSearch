#include <benchmark/benchmark.h>
#include "../src/search/search.h"
#include "../src/search/regex.h"
#include "../src/models/meta.h"
#include "../src/models/index.h"
#include "../src/models/shard.h"
#include "../src/start/updates.h"

#include <vector>

static void SearchIndex(benchmark::State &state) {
    // Perform setup here

    for (auto _ : state) {
        // This code gets timed
        RegexQuery query("integ");
        auto file_paths_by_id = LoadFilePathsById("static/netdata/files_path_by_id.bin");
        auto cnt_indexes = LoadCntIndexes("static/netdata/cnt_indexes.bin");

        std::vector<Index::IndexForSearch> indexes = LoadIndexes(cnt_indexes,
                                                                 "static/netdata/");
        Search(query, indexes, file_paths_by_id);
    }
}

static void CreateIndex(benchmark::State &state) {
    Meta meta("static/electron/meta_info.bin");
    for (auto _ : state) {
        // This code gets timed
        CreateIndex(meta);
    }
}
// Register the function as a benchmark
BENCHMARK(SearchIndex);
BENCHMARK(CreateIndex);

// Run the benchmark
BENCHMARK_MAIN();
