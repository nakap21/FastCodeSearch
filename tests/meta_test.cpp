#include "gtest/gtest.h"
#include "../src/models/meta.h"
#include "../src/meta/configs.h"
#include "../src/meta/files.h"

TEST(meta_test_case, files_test) {
    AddFiles(std::vector<std::string>{"static/files"});
    {
        Meta meta;
        EXPECT_EQ(meta.GetFiles().size(), 2);
    }
    DeleteFiles(std::vector<std::string>{"static/files/test.cpp"});
    {
        Meta meta;
        EXPECT_EQ(meta.GetFiles().size(), 1);
    }
    DeleteFiles(std::vector<std::string>{"static/files/test.cpp"});
    {
        Meta meta;
        EXPECT_EQ(meta.GetFiles().size(), 1);
    }
    AddFiles(std::vector<std::string>{"static/files"});
    {
        Meta meta;
        EXPECT_EQ(meta.GetFiles().size(), 2);
    }
    DeleteFiles(std::vector<std::string>{"static"});
    {
        Meta meta;
        EXPECT_EQ(meta.GetFiles().size(), 0);
    }
}

TEST(meta_test_case, configs_test) {
    UpdateConfig("CNT_FILES_IN_SHARD", std::vector<std::string>{"1000"});
    Meta meta;
    EXPECT_EQ(meta.GetCntFilesInShard().value, 1000);
}


TEST(meta_test_case, stop_test) {
    Meta meta;
    meta.StopEngine();
    EXPECT_EQ(meta.ShouldStopEngine(), true);
}