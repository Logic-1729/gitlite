#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <string>
#include "Utils.h"

class Repository {
public:
    // Current working directory
    static std::string getCWD();
    
    // The .gitlet directory
    static std::string getGitletDir();
    static std::string getObjectsDir();
    static std::string getCommitsDir();
    static std::string getBranchDir();
    static std::string getRemoteDir();
    static std::string getStageFile();
    static std::string getHeadFile();

private:
    static std::string CWD;
    static std::string GITLET_DIR;
    static std::string OBJECTS_DIR;
    static std::string COMMITS_DIR;
    static std::string BRANCH_DIR;
    static std::string REMOTE_DIR;
    static std::string STAGE_FILE;
    static std::string HEAD_FILE;
    
    static void initializePaths();
    static bool pathsInitialized;
};

#endif // REPOSITORY_H
