#ifndef BRANCH_H
#define BRANCH_H

#include <string>

class Branch {
public:
    static void setCommitId(const std::string& branchName, const std::string& commitId);
    static void setCommitId2(const std::string& name, const std::string& branchName, const std::string& commitId);
    static std::string getCommitId(const std::string& branchName);
    static void setRemoteCommitId(const std::string& remoteGitPath, const std::string& remoteBranchName, const std::string& commitId);
    static std::string getRemoteCommitId(const std::string& remoteGitPath, const std::string& remoteBranchName);
};

#endif // BRANCH_H
