#ifndef SOME_OBJ_H
#define SOME_OBJ_H

#include <string>
#include <vector>
#include <set>
#include <map>

class SomeObj {
public:
    void init();
    void add(const std::string& fileName);
    static void commit(const std::string& message);
    static void mergeCommit(const std::string& message, const std::string& head, const std::string& another);
    void rm(const std::string& fileName);
    void log();
    void globalLog();
    void find(const std::string& commitMessage);
    void status();
    void checkoutFile(const std::string& fileName);
    void checkoutFileInCommit(const std::string& commitId, const std::string& fileName);
    void checkoutBranch(const std::string& branchName);
    void branch(const std::string& branchName);
    void rmBranch(const std::string& branchName);
    void reset(const std::string& commitId);
    void merge(const std::string& branchName);
    void addRemote(const std::string& remoteName, const std::string& remotePath);
    void rmRemote(const std::string& remoteName);
    void push(const std::string& remoteName, const std::string& remoteBranchName);
    void fetch(const std::string& remoteName, const std::string& remoteBranchName);
    void pull(const std::string& remoteName, const std::string& remoteBranchName);

private:
    bool isConflict = false;
    std::set<std::string> skipFiles;
    
    // Helper methods
    static void checkoutCommit(const class Commit& commit);
    void splitPointCheck(const class Commit& headCommit, const class Commit& branchCommit, const class Commit& splitPointCommit);
    std::string conflictFileContents(const std::string& headBlobId, const std::string& branchBlobId);
    void notSplitCheck(const std::map<std::string, std::string>& splitPointTree,
                       const std::map<std::string, std::string>& headCommitTree,
                       const std::map<std::string, std::string>& branchCommitTree,
                       const class Commit& branchCommit,
                       const std::set<std::string>& skipFiles);
    std::string findAncestor(const std::string& headCommitId, const std::string& targetCommitId);
    std::vector<std::string> sortMapNames(const std::map<std::string, std::string>& map);
    std::vector<std::string> sortSetNames(const std::set<std::string>& set);
    static std::string valueToKey(const std::map<std::string, std::string>& map, const std::string& name);
    void ensureRemoteDirExists();
    std::string branchNameToBranchFile(const std::string& branchName);
};

#endif // SOME_OBJ_H
