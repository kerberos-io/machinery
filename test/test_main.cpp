#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv)
{
    el::Configurations config;
    config.setToDefault();
    config.set(el::Level::Info, el::ConfigurationType::Enabled, "false");
    el::Loggers::reconfigureAllLoggers(config);

	  ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
