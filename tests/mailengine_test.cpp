#include <gtest/gtest.h>
#include "mailengine.h"
#include "loglibrary.h"
#include <chrono>

TEST(MailEngineSuite, NOOP_Request){
    GTEST_SKIP();
    ImapRequest ir{};
    EXPECT_TRUE(ir.NOOP()) << "End of the world. NOOP failed.";
}

TEST(MailEngineSuite, CAPABILITY_Request){
    GTEST_SKIP();
    ImapRequest ir{};
    Capability ret = ir.CAPABILITY();
    EXPECT_TRUE(ret.isSuccess()) << "Capability request failed. Missing config, or broken internet?";
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
        bool found = (std::find(ret.getCapabilities().begin(), ret.getCapabilities().end(), exp_cap) != ret.getCapabilities().end());
        EXPECT_TRUE(found) << "Could not find " << exp_cap << " in the capabilities returned!";
    }
}

TEST(MailEngineSuite, ENABLE_Request){
    GTEST_SKIP() << "GreenMail does not support ENABLE request";
    ImapRequest ir{};
    bool ret = ir.ENABLE("UIDPLUS");
    EXPECT_TRUE(ret) << "ENABLE command wasn't accepted.";
}

TEST(MailEngineSuite, EXAMINE_Request){
    GTEST_SKIP();
    ImapRequest ir{};
    Examine ret = ir.EXAMINE("INBOX");
    EXPECT_TRUE(ret.isSuccess()) << "Unsuccessful EXAMINE request!";
    EXPECT_EQ(ret.getExists(), 2) << "Incorrect folder status";
    EXPECT_EQ(ret.getRecent(), 2) << "Incorrect folder status";
    EXPECT_EQ(ret.getUidNext(), 3) << "Incorrect folder status";

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
    MailEngine me{};
    FolderList ret = me.folderList();
    auto folders = ret.getFolderList();
    EXPECT_TRUE(ret.isSuccess()) << "Could not query folder list!";
    EXPECT_EQ(folders.at(0), "INBOX") << "Incorrect folder content, there is no INBOX at the start!";
    EXPECT_TRUE(std::find(folders.begin(), folders.end(), "space in name") != folders.end()) << "Could not find 'space in name' mailbox!";
    EXPECT_FALSE(std::find(folders.begin(), folders.end(), "spacein name") != folders.end()) << "Found a mailbox that should not exist. That doesn't sound correct.";
}

TEST(MailEngineSuite, FETCH_uid_request){
    GTEST_SKIP();
    ImapRequest ir{};
    MailEngine me{};
    auto examineResponse = ir.EXAMINE("INBOX");
    int lastMessageIndex = examineResponse.getExists();
    auto response = me.getMessageUid("INBOX", lastMessageIndex);
    EXPECT_TRUE(response.isSuccess()) << "Could not get the UID of the test message!";
    EXPECT_EQ(response.getUid(), 2) << "Incorect UID for the test message: " << response.getUid();
}

TEST(MailEngineSuite, FETCH_uid_wrong_folder){
    GTEST_SKIP();
    ImapRequest ir{};
    MailEngine me{};
    auto examineresponse = ir.EXAMINE("INBOX");
    int lastMessageIndex = examineresponse.getExists();
    auto response = me.getMessageUid("BAD_INBOX", lastMessageIndex);
    EXPECT_FALSE(response.isSuccess()) << "Incorrect UID response, it somehow succeeded?";
}

TEST(MailEngineSuite, FETCH_uid_wrong_messageindex){
    GTEST_SKIP();
    ImapRequest ir{};
    MailEngine me{};
    auto examineresponse = ir.EXAMINE("INBOX");
    int lastMessageIndex = examineresponse.getExists();
    auto response = me.getMessageUid("INBOX", lastMessageIndex + 5);
    EXPECT_FALSE(response.isSuccess()) << "Incorrect UID response, it succeeded with incorrect mailindex!";
}

TEST(MailEngineSuite, FETCH_header_request){
    GTEST_SKIP();
    ImapRequest ir{};
    MailEngine me{};
    auto examineresponse = ir.EXAMINE("INBOX");
    int lastMessageIndex = examineresponse.getExists();
    MailHeader headers = me.getMessageHeaders("INBOX", lastMessageIndex);
    EXPECT_EQ(headers.getFrom(), "dick.grayson@gotham.us");
    EXPECT_EQ(headers.getSubject(), "Mail with Attachment");
    EXPECT_EQ(headers.getTo().at(0), "diana.prince@wonderwoman.com");
    EXPECT_EQ(headers.getTo().size(), 1);

    auto current_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    auto yesterday = current_time - (24 * 3600);
    EXPECT_TRUE(headers.getDate() > yesterday);
    EXPECT_TRUE(headers.getDate() < current_time);

}

TEST(MailEngineSuite, FETCH_bodystructure_request){
    GTEST_SKIP();
    ImapRequest ir{};
    MailEngine me{};
    auto examineresponse = ir.EXAMINE("INBOX");
    int lastMessageIndex = examineresponse.getExists();
    BodyStructures ret = me.getBodyStructure("INBOX", lastMessageIndex);

    EXPECT_TRUE(ret.getBodyStructures().at(0).isSuccess()) << "Could not retrieve bodystructure!";
    EXPECT_EQ(ret.getBodyStructures().at(0).getType(), "TEXT") << "Incorrect bodystructure type!";
    EXPECT_EQ(ret.getBodyStructures().at(0).getSubType(), "PLAIN") << "Incorrect bodystructure subtype!";
    EXPECT_EQ(ret.getBodyStructures().at(0).getParameters()["charset"], "us-ascii") << "Incorrect parameter list in bodystructure!";
    EXPECT_EQ(ret.getBodyStructures().at(0).getId(), "") << "Incorrect bodystructure id!";
    EXPECT_EQ(ret.getBodyStructures().at(0).getDescription(), "") << "Incorrect bodystructure description!";
    EXPECT_EQ(ret.getBodyStructures().at(0).getEncoding(), "7bit") << "Incorrect bodystructrure encoding!";
    EXPECT_EQ(ret.getBodyStructures().at(0).getSize(), 50) << "Incorrect size in bodystructure!";
    EXPECT_EQ(ret.getBodyStructures().at(0).getMd5(), "1") << "Unexpected MD5 in bodystructure!";
    EXPECT_EQ(ret.getBodyStructures().at(0).getDisposition(), "") << "Incorrect disposition in bodystructure!";
    EXPECT_EQ(ret.getBodyStructures().at(0).getLanguage(), "") << "Incorrect language in bodystructure!";
    EXPECT_EQ(ret.getBodyStructures().at(0).getLocation(), "") << "Incorrect location in bodystructure!";
}


TEST(MailEngineSuite, FETCH_textbody_request){
    GTEST_SKIP();
    ImapRequest ir{};
    MailEngine me{};
    auto examineResponse = ir.EXAMINE("INBOX");
    int lastMessageIndex = examineResponse.getExists();
    std::string ret = me.getTextBody("INBOX", lastMessageIndex);
    std::string expectedResponse = "There should be a text attachment somewhere around";
    EXPECT_EQ(ret, expectedResponse);
}

TEST(MailEngineSuite, FETCH_flags){
    GTEST_SKIP();
    ImapRequest ir{};
    MailEngine me{};
    auto examineResponse =ir.EXAMINE("INBOX");
    int lastMessageIndex = examineResponse.getExists();
    auto uid = me.getMessageUid("INBOX", lastMessageIndex);
    auto a = me.getMessageFlags("INBOX", lastMessageIndex);
    auto b = me.getMessageHeaders("INBOX", lastMessageIndex);
    ERROR("subject: {}", b.getSubject());
}

/*
* There is one strange email that is not fetched correctly from
* my inbox. This test is for debugging it - would be disabled soon enough.
*/
TEST(MailEngineSuite, FETCH_problemtic_email){
    MailEngine me{};
    uint32_t problematicIndex = 6653;
    std::string folder = "INBOX";
    bool ignoreCache = true;
    auto mail = me.getMail(folder, problematicIndex, ignoreCache);
    LOG("DONE");
}
