#include <gtest/gtest.h>
#include "mailengine2.h"
#include "loglibrary.h"
#include <chrono>

TEST(MailEngineSuite, NOOP_Request){
    GTEST_SKIP();
    MailEngine2 me2{};
    EXPECT_TRUE(me2.NOOP()) << "End of the world. NOOP failed.";
}

TEST(MailEngineSuite, CAPABILITY_Request){
    GTEST_SKIP();
    MailEngine2 me2{};
    ImapStringVectorResponse ret = me2.CAPABILITY();
    EXPECT_TRUE(ret.success) << "Capability request failed. Missing config, or broken internet?";
// IMAP4rev1 LITERAL+ UIDPLUS SORT IDLE MOVE QUOTA
    std::vector<std::string> expected_capabilities;
    // the commented ones are present in GMail response, but not in GreenMail
    expected_capabilities.push_back("IMAP4rev1");
    //expected_capabilities.push_back("UNSELECT");
    expected_capabilities.push_back("IDLE");
    //expected_capabilities.push_back("NAMESPACE");
    expected_capabilities.push_back("QUOTA");
    //expected_capabilities.push_back("ID");
    //expected_capabilities.push_back("XLIST");
    //expected_capabilities.push_back("CHILDREN");
    //expected_capabilities.push_back("X-GM-EXT-1");
    expected_capabilities.push_back("UIDPLUS");
    //expected_capabilities.push_back("COMPRESS=DEFLATE");
    //expected_capabilities.push_back("ENABLE");
    expected_capabilities.push_back("MOVE");
    //expected_capabilities.push_back("CONDSTORE");
    //expected_capabilities.push_back("ESEARCH");
    //expected_capabilities.push_back("UTF8=ACCEPT");
    //expected_capabilities.push_back("LIST-EXTENDED");
    //expected_capabilities.push_back("LIST-STATUS");
    //expected_capabilities.push_back("LITERAL-");
    //expected_capabilities.push_back("SPECIAL-USE");

    for (const auto& exp_cap: expected_capabilities){
        bool found = (std::find(ret.value.begin(), ret.value.end(), exp_cap) != ret.value.end());
        EXPECT_TRUE(found) << "Could not find " << exp_cap << " in the capabilities returned!";
    }
}

TEST(MailEngineSuite, ENABLE_Request){
    GTEST_SKIP();
    MailEngine2 me2{};
    bool ret = me2.ENABLE("UIDPLUS");
    EXPECT_TRUE(ret) << "ENABLE command wasn't accepted.";
}

TEST(MailEngineSuite, EXAMINE_Request){
    GTEST_SKIP();
    MailEngine2 me2{};
    Examine ret = me2.EXAMINE("INBOX");
    EXPECT_TRUE(ret.isSuccess()) << "Unsuccessful EXAMINE request!";
    EXPECT_EQ(ret.getExists(), 1) << "Incorrect folder status";
    EXPECT_EQ(ret.getRecent(), 1) << "Incorrect folder status";
    EXPECT_EQ(ret.getUidNext(), 2) << "Incorrect folder status";

    std::vector<std::string> expectedFlags;
    expectedFlags.push_back("\\Answered");
    expectedFlags.push_back("\\Deleted");
    expectedFlags.push_back("\\Draft");
    expectedFlags.push_back("\\Flagged");
    expectedFlags.push_back("\\Seen");
    expectedFlags.push_back("\\*");

    for (const std::string& s: expectedFlags) {
        EXPECT_TRUE(std::find(ret.getFlags().begin(), ret.getFlags().end(), s) != ret.getFlags().end()) << s << " flag not found in flags!";
        EXPECT_TRUE(std::find(ret.getPermanentFlags().begin(), ret.getPermanentFlags().end(), s) != ret.getPermanentFlags().end()) << s << " flag not found in permanent flags!";
    }
}

TEST(MailEngineSuite, LIST_Folders_Request){
    GTEST_SKIP();
    MailEngine2 me2{};
    ImapStringVectorResponse ret = me2.folderList();
    EXPECT_TRUE(ret.success) << "Could not query folder list!";
    EXPECT_EQ(ret.value.at(0), "INBOX") << "Incorrect folder content, there is no INBOX at the start!";
    EXPECT_TRUE(std::find(ret.value.begin(), ret.value.end(), "space in name") != ret.value.end()) << "Could not find 'space in name' mailbox!";
    EXPECT_FALSE(std::find(ret.value.begin(), ret.value.end(), "spacein name") != ret.value.end()) << "Found a mailbox that should not exist. That doesn't sound correct.";
}

TEST(MailEngineSuite, FETCH_uid_request){
    GTEST_SKIP();
    MailEngine2 me2{};
    auto examineResponse = me2.EXAMINE("INBOX");
    int lastMessageIndex = examineResponse.getExists();
    auto response = me2.getMessageUid("INBOX", lastMessageIndex);
    EXPECT_TRUE(response.success) << "Could not get the UID of the test message!";
    EXPECT_EQ(response.value, 1) << "Incorect UID for the test message: " << response.value;
}

TEST(MailEngineSuite, FETCH_uid_wrong_folder){
    GTEST_SKIP();
    MailEngine2 me2{};
    auto examineresponse = me2.EXAMINE("INBOX");
    int lastMessageIndex = examineresponse.getExists();
    auto response = me2.getMessageUid("BAD_INBOX", lastMessageIndex);
    EXPECT_FALSE(response.success) << "Incorrect UID response, it somehow succeeded?";
}

TEST(MailEngineSuite, FETCH_uid_wrong_messageindex){
    GTEST_SKIP();
    MailEngine2 me2{};
    auto examineresponse = me2.EXAMINE("INBOX");
    int lastMessageIndex = examineresponse.getExists();
    auto response = me2.getMessageUid("INBOX", lastMessageIndex + 5);
    EXPECT_FALSE(response.success) << "Incorrect UID response, it succeeded with incorrect mailindex!";
}

TEST(MailEngineSuite, FETCH_header_request){
    GTEST_SKIP();
    MailEngine2 me2{};
    auto examineresponse = me2.EXAMINE("INBOX");
    int lastMessageIndex = examineresponse.getExists();
    ImapHeaderResponse headers = me2.getMessageHeaders("INBOX", lastMessageIndex);
    EXPECT_EQ(headers.from, "dick.grayson@gotham.us");
    EXPECT_EQ(headers.subject, "Batman is evil!");
    EXPECT_EQ(headers.to.at(0), "diana.prince@wonderwoman.com");
    EXPECT_EQ(headers.to.at(1), "clarke.kent@superman.com");

    auto current_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    auto yesterday = current_time - (24 * 3600);
    EXPECT_TRUE(headers.date > yesterday);
    EXPECT_TRUE(headers.date < current_time);

}

TEST(MailEngineSuite, FETCH_bodystructure_request){

    MailEngine2 me2{};
    auto examineresponse = me2.EXAMINE("INBOX");
    int lastMessageIndex = examineresponse.getExists();
    auto ret = me2.getBodyStructure("INBOX", lastMessageIndex);

    EXPECT_TRUE(ret.at(0).success) << "Could not retrieve bodystructure!";
    EXPECT_EQ(ret.at(0).type, "TEXT") << "Incorrect bodystructure type!";
    EXPECT_EQ(ret.at(0).subType, "PLAIN") << "Incorrect bodystructure subtype!";
    EXPECT_EQ(ret.at(0).parameters["charset"], "us-ascii") << "Incorrect parameter list in bodystructure!";
    EXPECT_EQ(ret.at(0).id, "") << "Incorrect bodystructure id!";
    EXPECT_EQ(ret.at(0).description, "") << "Incorrect bodystructure description!";
    EXPECT_EQ(ret.at(0).encoding, "7bit") << "Incorrect bodystructrure encoding!";
    EXPECT_EQ(ret.at(0).size, 45) << "Incorrect size in bodystructure!";
    EXPECT_EQ(ret.at(0).md5, "1") << "Unexpected MD5 in bodystructure!";
    EXPECT_EQ(ret.at(0).disposition, "") << "Incorrect disposition in bodystructure!";
    EXPECT_EQ(ret.at(0).language, "") << "Incorrect language in bodystructure!";
    EXPECT_EQ(ret.at(0).location, "") << "Incorrect location in bodystructure!";
}


TEST(MailEngineSuite, FETCH_textbody_request){
    GTEST_SKIP();
    MailEngine2 me2{};
    auto examineresponse = me2.EXAMINE("INBOX");
    int lastMessageIndex = examineresponse.getExists();
    auto ret = me2.getTextBody("INBOX", lastMessageIndex);
    EXPECT_TRUE(false) << ret.value;
}
