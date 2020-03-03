//
// Created by joybin on 2020/3/3.
//

#include "gtest/gtest.h"
#include "utf8.hh"

// The fixture for testing class UTF8.
class UTF8Test : public ::testing::Test {

protected:

    // You can do set-up work for each test here.
    UTF8Test();

    // You can do clean-up work that doesn't throw exceptions here.
    virtual ~UTF8Test();

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();

};

UTF8Test::UTF8Test() {}
UTF8Test::~UTF8Test() {}
void UTF8Test::SetUp() {}
void UTF8Test::TearDown() {}

TEST_F(UTF8Test, ByDefaultBazTrueIsTrue) {
    EXPECT_STREQ(Utf8::encode(L"hello").data(), "hello");
}