#include "../include/HEAD.h"
#include "../include/Repository.h"
#include "../include/Utils.h"

void HEAD::setBranchName(const std::string& branchName) {
    Utils::writeContents(Repository::getHeadFile(), branchName);
}

std::string HEAD::getBranchName() {
    return Utils::readContentsAsString(Repository::getHeadFile());
}
