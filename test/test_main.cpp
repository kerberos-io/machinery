#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "easylogging++.h"

_INITIALIZE_EASYLOGGINGPP
    
int main(int argc, char** argv)
{
    easyloggingpp::Configurations config;
    config.setAll(easyloggingpp::ConfigurationType::Enabled , "false");
    easyloggingpp::Loggers::reconfigureAllLoggers(config);
    
	::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}