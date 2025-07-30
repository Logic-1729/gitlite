#include "../include/Repository.h"
#include <unistd.h>
#include <limits.h>

std::string Repository::CWD;
std::string Repository::GITLITE_DIR;
std::string Repository::OBJECTS_DIR;
std::string Repository::COMMITS_DIR;
std::string Repository::BRANCH_DIR;
std::string Repository::REMOTE_DIR;
std::string Repository::STAGE_FILE;
std::string Repository::HEAD_FILE;
bool Repository::pathsInitialized = false;

void Repository::initializePaths() {
    if (pathsInitialized) return;
    
    char buffer[PATH_MAX];
    if (getcwd(buffer, sizeof(buffer)) != nullptr) {
        CWD = std::string(buffer);
    } else {
        CWD = ".";
    }

    GITLITE_DIR = Utils::join(CWD, ".gitlite");
    OBJECTS_DIR = Utils::join(GITLITE_DIR, ".objects");
    COMMITS_DIR = Utils::join(GITLITE_DIR, ".commits");
    BRANCH_DIR = Utils::join(GITLITE_DIR, ".branch");
    REMOTE_DIR = Utils::join(GITLITE_DIR, ".remote");
    STAGE_FILE = Utils::join(GITLITE_DIR, "stagingArea");
    HEAD_FILE = Utils::join(GITLITE_DIR, "HEAD");

    pathsInitialized = true;
}

std::string Repository::getCWD() {
    initializePaths();
    return CWD;
}

std::string Repository::getGitliteDir() {
    initializePaths();
    return GITLITE_DIR;
}

std::string Repository::getObjectsDir() {
    initializePaths();
    return OBJECTS_DIR;
}

std::string Repository::getCommitsDir() {
    initializePaths();
    return COMMITS_DIR;
}

std::string Repository::getBranchDir() {
    initializePaths();
    return BRANCH_DIR;
}

std::string Repository::getRemoteDir() {
    initializePaths();
    return REMOTE_DIR;
}

std::string Repository::getStageFile() {
    initializePaths();
    return STAGE_FILE;
}

std::string Repository::getHeadFile() {
    initializePaths();
    return HEAD_FILE;
}
