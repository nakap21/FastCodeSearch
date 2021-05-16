#include "../models/meta.h"
#include "../models/shard.h"
#include "updates.h"

#include <chrono>
#include <iostream>
#include <thread>

int main() {
    try {
        Meta meta;
        auto shards = CreateIndex(meta);
        while (!meta.ShouldStopEngine()) {
            if (ShouldUpdateMeta(meta)) {
                UpdateMeta(meta);
                shards.Clear();
                shards = CreateIndex(meta);
            }
            if (ShouldUpdateIndex(meta)) {
                UpdateIndex(shards, meta);
            }
            std::this_thread::sleep_for(std::chrono::seconds(meta.GetUpdateIntervalSec().value));
        }
        shards.Clear();
        meta.Clear();
    } catch (const std::exception &ex) {
        std::cout << ex.what() << std::endl;
    }
}
