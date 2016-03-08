#include <limits.h>
#include "gtest/gtest.h"
#include <Helper.h>

using namespace std;

TEST(HELPER, T_TO_STRING)
{
    int number = 5;
    std::string word = kerberos::helper::to_string(number);
    EXPECT_EQ("5",word);
}

TEST(HELPER, NORMALIZE_PATH)
{
    std::string path = kerberos::helper::normalizePath("/home/kerberos/", "./bin/kerberos", "/bin/kerberos");
    EXPECT_EQ("/home/kerberos",path);

    path = kerberos::helper::normalizePath("/home/kerberos/build", "../bin/kerberos", "/bin/kerberos");
    EXPECT_EQ("/home/kerberos",path);

    path = kerberos::helper::normalizePath("/usr/local/", "../../home/kerberos/bin/kerberos", "/bin/kerberos");
    EXPECT_EQ("/home/kerberos",path);
}