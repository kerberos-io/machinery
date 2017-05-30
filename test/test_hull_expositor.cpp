#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Factory.h"
#include "Types.h"

#include "capture/MockImage.h"

using ::testing::AtLeast;
using ::testing::Return;
using namespace kerberos;

// ----------------------------------------
// Hull Expositor: Calculate function

TEST(HULL_EXPOSITOR, CALCULATE)
{
    Expositor * expositor = Factory<Expositor>::getInstance()->create("Hull");

    // -----------------
    // Create triangle hull
    //  - the positions x, are the positions that will be 
    //    evaluated by the hull expositor.
    //
    //  x   0   0
    //  x   x   x
    //  x   0   0

    StringMap settings;
    settings["expositors.Hull.region"] = "0,0|2,1|0,2";
    settings["capture.width"] = "3";
    settings["capture.height"] = "3";
    expositor->setup(settings);

    // ----------------------------
    // Mocking following image
    //
    //  0   0   0
    // 255 255  0
    //  0  255 255

    MockImage image;
    EXPECT_CALL(image, getColumns()).Times(AtLeast(1)).WillRepeatedly(Return(3));
    EXPECT_CALL(image, getRows()).Times(AtLeast(1)).WillRepeatedly(Return(3));
    EXPECT_CALL(image, get(testing::_, testing::_))
    // column is prior (left->right, top->bottom)
    .WillOnce(Return(0))
    .WillOnce(Return(255)).WillOnce(Return(255)).WillOnce(Return(0))
    .WillOnce(Return(0));

    JSON json;
    json.SetObject();
    expositor->calculate(image, json);

    // ----------------------------
    // number of changes = 4

    EXPECT_EQ(2, json["numberOfChanges"].GetInt());

    // -----------------------------------------
    // position of rectangle (0,1) -> (2,2)

    EXPECT_EQ(0, json["regionCoordinates"][0].GetInt());
    EXPECT_EQ(1, json["regionCoordinates"][1].GetInt());
    EXPECT_EQ(1, json["regionCoordinates"][2].GetInt());
    EXPECT_EQ(1, json["regionCoordinates"][3].GetInt());
}