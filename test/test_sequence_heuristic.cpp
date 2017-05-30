#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Factory.h"
#include "Types.h"

using ::testing::AtLeast;
using ::testing::Return;
using namespace kerberos;

// ----------------------------------------
// Sequence Expositor: IsValid function

TEST(SEQUENCE_HEURISTIC, IS_VALID)
{
    Heuristic * heuristic = Factory<Heuristic>::getInstance()->create("Sequence");

    StringMap settings;
    settings["heuristics.Sequence.minimumChanges"] = "1";
    settings["heuristics.Sequence.minimumDuration"] = "2";
    settings["heuristics.Sequence.motionDelayTime"] = "1000";
    settings["heuristics.Sequence.noMotionDelayTime"] = "1000";
    heuristic->setup(settings);

    // ----------------------------
    // Mocking image

    Image image;
    ImageVector images;

    JSON json;
    JSON::AllocatorType& allocator = json.GetAllocator();
    
    json.SetObject();
    json.AddMember("numberOfChanges", 0, allocator);

    // --------------------
    // Nothing changed thus false.
    bool isValid = heuristic->isValid(image, images, json);
    EXPECT_EQ(false, isValid);

    // ---------------------
    // Changed, but this is only the first occurence thus again false.
    json.RemoveMember ("numberOfChanges");
    json.AddMember("numberOfChanges", 1, allocator);
    isValid = heuristic->isValid(image, images, json);
    EXPECT_EQ(false, isValid);

    // ---------------------
    // Changed and the second occurence thus true.
    isValid = heuristic->isValid(image, images, json);
    EXPECT_EQ(true, isValid);
}
