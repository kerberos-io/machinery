#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Factory.h"
#include "Types.h"

using ::testing::AtLeast;
using ::testing::Return;
using namespace kerberos;

// ----------------------------------------
// Enabled condition: allowed function

TEST(ENABLED_CONDITION, ALLOWED)
{
    Condition * condition = Factory<Condition>::getInstance()->create("Enabled");

    // This is only for mocking
   	ImageVector images;

    StringMap settings;
    settings["conditions.Enabled.delay"] = "2000";

    // Enable condition
    settings["conditions.Enabled.active"] = "true";
    condition->setup(settings);
    bool canExecute = condition->allowed(images);
    EXPECT_EQ(true, canExecute);

    // Disable condition
    settings["conditions.Enabled.active"] = "false";
    condition->setup(settings);
    canExecute = condition->allowed(images);
    EXPECT_EQ(false, canExecute);
}