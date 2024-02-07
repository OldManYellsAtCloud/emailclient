#ifndef MAILSTRUCTURE_H
#define MAILSTRUCTURE_H

#include <string>
#include <vector>

enum class PartType {
    TEXT,
    NA
};

enum class PartSubType {
    PLAIN,
    HTML,
    NA
};

struct MailPart{
    PartType partType;
    PartSubType partSubType;
    bool isAttachment;
    std::string id;
};

class MailStructure{
private:
    std::vector<MailPart> sections;
public:
    MailStructure(std::string structure);
    std::vector<MailPart> getAllSections();
};

#endif // MAILSTRUCTURE_H
