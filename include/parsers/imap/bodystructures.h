#ifndef BODYSTRUCTURES_H
#define BODYSTRUCTURES_H

#include <vector>
#include "bodystructure.h"

#define BODYSTRUCTURE_REGEX R"-(\((?!BODYSTRUCTURE \(*)(.*?)\)(?!\s+NIL))-"

class BodyStructures
{
    bool success;
    std::vector<BodyStructure> bodyStructures;
public:
    explicit BodyStructures(std::string s);
    bool isSuccess(){return success;}
    std::vector<BodyStructure> getBodyStructures(){return bodyStructures;}
};

#endif // BODYSTRUCTURES_H
