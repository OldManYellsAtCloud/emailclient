#include <gtest/gtest.h>
#include "utils.h"

TEST(MiscSuite, CaseInsenstiveStringCompare_1){
    std::string lowerString = "teststring";
    std::string upperString = "TESTSTRING";
    EXPECT_TRUE(iCompareString(lowerString, upperString));
}

TEST(MiscSuite, CaseInsenstiveStringCompare_2){
    std::string lowerString = "teststring";
    std::string upperString = "TESTSTRING2";
    EXPECT_FALSE(iCompareString(lowerString, upperString));
}

TEST(MiscSuite, CaseInsenstiveStringCompare_3){
    std::string upperString1 = "TESTSTRING";
    std::string upperString2 = "TESTSTRING";
    EXPECT_TRUE(iCompareString(upperString1, upperString2));
}

TEST(MiscSuite, CaseInsenstiveStringCompare_4){
    std::string lowerString = "teststring";
    std::string upperString = "XESTSTRING";
    EXPECT_FALSE(iCompareString(lowerString, upperString));
}
