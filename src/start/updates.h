#include "../models/meta.h"
#include "../models/shard.h"

Shards CreateIndex(const Meta &meta);

bool ShouldUpdateMeta(const Meta &meta);

void UpdateMeta(Meta &meta);

bool ShouldUpdateIndex(const Meta &meta);

void UpdateIndex(Shards &shards, Meta &meta);