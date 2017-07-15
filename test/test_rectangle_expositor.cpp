#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Factory.h"
#include "Types.h"

#include "capture/MockImage.h"

using ::testing::AtLeast;
using ::testing::Return;
using namespace kerberos;

// ----------------------------------------
// Rectangle Expositor: Calculate function

TEST(RECTANGLE_EXPOSITOR, CALCULATE)
{
    Expositor * expositor = Factory<Expositor>::getInstance()->create("Rectangle");

    // -----------------
    // Create rectangle
    //  - the positions x, are the positions that will be 
    //    evaluated by the rectangle expositor.
    //
    //  x   x   0
    //  x   x   0
    //  x   x   0

    StringMap settings;
    settings["expositors.Rectangle.region.x1"] = "0";
    settings["expositors.Rectangle.region.y1"] = "0";
    settings["expositors.Rectangle.region.x2"] = "2";
    settings["expositors.Rectangle.region.y2"] = "3";
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
    // row is prior (top->bottom, left->right)
    .WillOnce(Return(0)).WillOnce(Return(255)).WillOnce(Return(0))
    .WillOnce(Return(0)).WillOnce(Return(255)).WillOnce(Return(255));

    JSON json;
    json.SetObject();
    expositor->calculate(image, json);

    // ----------------------------
    // number of changes = 4

    EXPECT_EQ(3, json["numberOfChanges"].GetInt());

    // -----------------------------------------
    // position of rectangle (0,1) -> (2,2)

    EXPECT_EQ(0, json["regionCoordinates"][0].GetInt());
    EXPECT_EQ(1, json["regionCoordinates"][1].GetInt());
    EXPECT_EQ(1, json["regionCoordinates"][2].GetInt());
    EXPECT_EQ(2, json["regionCoordinates"][3].GetInt());
}