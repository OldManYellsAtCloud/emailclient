#include "MailStructure.h"
#include <gtest/gtest.h>

TEST(MailStructureTestSuite, SingleParsing1) {
    std::string bodyStructure = "* 3701 FETCH (UID 6592 BODYSTRUCTURE ((\"TEXT\" \"PLAIN\" (\"CHARSET\" \"UTF-8\") NIL NIL \"7BIT\" 143 6 NIL NIL NIL))";
    MailStructure ms {bodyStructure};
    auto sections = ms.getAllSections();
    ASSERT_EQ(sections.size(), 1) << "The size of sections is not 1, but " << sections.size();
    ASSERT_EQ(sections.at(0).id, "") << "Incorrect parsed ID!";
    ASSERT_EQ(sections.at(0).isAttachment, false) << "Incorrect attachment detection!";
    ASSERT_EQ(sections.at(0).partType, PartType::TEXT) << "Incorrect part type detection!";
    ASSERT_EQ(sections.at(0).partSubType, PartSubType::PLAIN) << "Incorrect subType detection!";
}

TEST(MailStructureTestSuite, SingleParsing2) {
    std::string bodyStructure = "* 3701 FETCH (UID 6592 BODYSTRUCTURE ((\"TEXT\" \"HTML\" (\"CHARSET\" \"UTF-8\") NIL NIL \"7BIT\" 143 6 NIL NIL NIL))";
    MailStructure ms {bodyStructure};
    auto sections = ms.getAllSections();
    ASSERT_EQ(sections.size(), 1) << "The size of sections is not 1, but " << sections.size();
    ASSERT_EQ(sections.at(0).id, "") << "Incorrect parsed ID!";
    ASSERT_EQ(sections.at(0).isAttachment, false) << "Incorrect attachment detection!";
    ASSERT_EQ(sections.at(0).partType, PartType::TEXT) << "Incorrect part type detection!";
    ASSERT_EQ(sections.at(0).partSubType, PartSubType::HTML) << "Incorrect subType detection!";
}

TEST(MailStructureTestSuite, UnknownType) {
    std::string bodyStructure = "* 3701 FETCH (UID 6592 BODYSTRUCTURE ((\"Non-existent\" \"HTML\" (\"CHARSET\" \"UTF-8\") NIL NIL \"7BIT\" 143 6 NIL NIL NIL))";
    MailStructure ms {bodyStructure};
    auto sections = ms.getAllSections();
    ASSERT_EQ(sections.size(), 1) << "The size of sections is not 1, but " << sections.size();
    ASSERT_EQ(sections.at(0).id, "") << "Incorrect parsed ID!";
    ASSERT_EQ(sections.at(0).isAttachment, false) << "Incorrect attachment detection!";
    ASSERT_EQ(sections.at(0).partType, PartType::NA) << "Incorrect part type detection!";
    ASSERT_EQ(sections.at(0).partSubType, PartSubType::HTML) << "Incorrect subType detection!";
}

TEST(MailStructureTestSuite, UnknownSubType) {
    std::string bodyStructure = "* 3701 FETCH (UID 6592 BODYSTRUCTURE ((\"TEXT\" \"Non-existent\" (\"CHARSET\" \"UTF-8\") NIL NIL \"7BIT\" 143 6 NIL NIL NIL))";
    MailStructure ms {bodyStructure};
    auto sections = ms.getAllSections();
    ASSERT_EQ(sections.size(), 1) << "The size of sections is not 1, but " << sections.size();
    ASSERT_EQ(sections.at(0).id, "") << "Incorrect parsed ID!";
    ASSERT_EQ(sections.at(0).isAttachment, false) << "Incorrect attachment detection!";
    ASSERT_EQ(sections.at(0).partType, PartType::TEXT) << "Incorrect part type detection!";
    ASSERT_EQ(sections.at(0).partSubType, PartSubType::NA) << "Incorrect subType detection!";
}

TEST(MailStructureTestSuite, ParseId) {
    std::string bodyStructure = "* 3701 FETCH (UID 6592 BODYSTRUCTURE ((\"TEXT\" \"PLAIN\" (\"CHARSET\" \"UTF-8\") \"my-custom-id\" NIL \"7BIT\" 143 6 NIL NIL NIL))";
    MailStructure ms {bodyStructure};
    auto sections = ms.getAllSections();
    ASSERT_EQ(sections.size(), 1) << "The size of sections is not 1, but " << sections.size();
    ASSERT_EQ(sections.at(0).id, "my-custom-id") << "Incorrect parsed ID!";
    ASSERT_EQ(sections.at(0).isAttachment, false) << "Incorrect attachment detection!";
    ASSERT_EQ(sections.at(0).partType, PartType::TEXT) << "Incorrect part type detection!";
    ASSERT_EQ(sections.at(0).partSubType, PartSubType::PLAIN) << "Incorrect subType detection!";
}

TEST(MailStructureTestSuite, ParseMultipleParts) {
    std::string bodyStructure = "* 3701 FETCH (UID 6592 BODYSTRUCTURE ((\"TEXT\" \"PLAIN\" (\"CHARSET\" \"UTF-8\") \"text-part\" NIL \"7BIT\" 143 6 NIL NIL NIL)\
                                (\"TEXT\" \"HTML\" (\"CHARSET\" \"UTF-8\") \"html-part\" NIL \"7BIT\" 143 6 NIL NIL NIL))";
    MailStructure ms {bodyStructure};
    auto sections = ms.getAllSections();
    ASSERT_EQ(sections.size(), 2) << "The size of sections is not 3, but " << sections.size();
    ASSERT_EQ(sections.at(0).id, "text-part") << "Incorrect parsed ID!";
    ASSERT_EQ(sections.at(0).isAttachment, false) << "Incorrect attachment detection!";
    ASSERT_EQ(sections.at(0).partType, PartType::TEXT) << "Incorrect part type detection!";
    ASSERT_EQ(sections.at(0).partSubType, PartSubType::PLAIN) << "Incorrect subType detection!";

    ASSERT_EQ(sections.at(1).id, "html-part") << "Incorrect parsed ID!";
    ASSERT_EQ(sections.at(1).isAttachment, false) << "Incorrect attachment detection!";
    ASSERT_EQ(sections.at(1).partType, PartType::TEXT) << "Incorrect part type detection!";
    ASSERT_EQ(sections.at(1).partSubType, PartSubType::HTML) << "Incorrect subType detection!";
}

TEST(MailStructureTestSuite, ParseMultiplePartsWithoutIds) {
    std::string bodyStructure = "* 3701 FETCH (UID 6592 BODYSTRUCTURE ((\"TEXT\" \"PLAIN\" (\"CHARSET\" \"UTF-8\") NIL NIL \"7BIT\" 143 6 NIL NIL NIL)\
                                (\"TEXT\" \"HTML\" (\"CHARSET\" \"UTF-8\") NIL NIL \"7BIT\" 143 6 NIL NIL NIL))";
    MailStructure ms {bodyStructure};
    auto sections = ms.getAllSections();
    ASSERT_EQ(sections.size(), 2) << "The size of sections is not 3, but " << sections.size();
    ASSERT_EQ(sections.at(0).id, "") << "Incorrect parsed ID!";
    ASSERT_EQ(sections.at(0).isAttachment, false) << "Incorrect attachment detection!";
    ASSERT_EQ(sections.at(0).partType, PartType::TEXT) << "Incorrect part type detection!";
    ASSERT_EQ(sections.at(0).partSubType, PartSubType::PLAIN) << "Incorrect subType detection!";

    ASSERT_EQ(sections.at(1).id, "") << "Incorrect parsed ID!";
    ASSERT_EQ(sections.at(1).isAttachment, false) << "Incorrect attachment detection!";
    ASSERT_EQ(sections.at(1).partType, PartType::TEXT) << "Incorrect part type detection!";
    ASSERT_EQ(sections.at(1).partSubType, PartSubType::HTML) << "Incorrect subType detection!";
}

TEST(MailStructureTestSuite, ParseMultiplePartsWithOneAttachment) {
    std::string bodyStructure = "* 3701 FETCH (UID 6592 BODYSTRUCTURE ((\"TEXT\" \"PLAIN\" (\"CHARSET\" \"UTF-8\") \"text-part\" NIL \"7BIT\" 143 6 NIL NIL NIL)\
                                (\"TEXT\" \"HTML\" (\"CHARSET\" \"UTF-8\" \"NAME\" \"attachment-name\") \"attachment-part\" NIL \"7BIT\" 143 6 NIL NIL NIL))";
    MailStructure ms {bodyStructure};
    auto sections = ms.getAllSections();
    ASSERT_EQ(sections.size(), 2) << "The size of sections is not 3, but " << sections.size();
    ASSERT_EQ(sections.at(0).id, "text-part") << "Incorrect parsed ID!";
    ASSERT_EQ(sections.at(0).isAttachment, false) << "Incorrect attachment detection!";
    ASSERT_EQ(sections.at(0).partType, PartType::TEXT) << "Incorrect part type detection!";
    ASSERT_EQ(sections.at(0).partSubType, PartSubType::PLAIN) << "Incorrect subType detection!";

    ASSERT_EQ(sections.at(1).id, "attachment-part") << "Incorrect parsed ID!";
    ASSERT_EQ(sections.at(1).isAttachment, true) << "Incorrect attachment detection!";
    ASSERT_EQ(sections.at(1).partType, PartType::TEXT) << "Incorrect part type detection!";
    ASSERT_EQ(sections.at(1).partSubType, PartSubType::HTML) << "Incorrect subType detection!";
}
