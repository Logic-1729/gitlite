#include "../include/Branch.h"
#include "../include/Repository.h"
#include "../include/Utils.h"

void Branch::setCommitId(const std::string& branchName, const std::string& commitId) {
    std::string branchFile = Utils::join(Repository::getBranchDir(), branchName);
    Utils::writeContents(branchFile, commitId);
}

void Branch::setCommitId2(const std::string& name, const std::string& branchName, const std::string& commitId) {
    std::string nameDir = Utils::join(Repository::getBranchDir(), name);
    if (!Utils::isDirectory(nameDir)) Utils::createDirectories(nameDir);
    std::string branchFile = Utils::join(nameDir, branchName);
    Utils::writeContents(branchFile, commitId);
}

std::string Branch::getCommitId(const std::string& branchName) {
    std::string branchFile;
    
    if (branchName.find("/") != std::string::npos) {
        // Handle remote branch names like "origin/master"
        size_t pos = branchName.find("/");
        std::string remoteName = branchName.substr(0, pos);
        std::string remoteBranchName = branchName.substr(pos + 1);
        branchFile = Utils::join(Repository::getBranchDir(), remoteName, remoteBranchName);
    } else {
        branchFile = Utils::join(Repository::getBranchDir(), branchName);
    }
    
    if (!Utils::exists(branchFile)) return "";
    return Utils::readContentsAsString(branchFile);
}

void Branch::setRemoteCommitId(const std::string& remoteGitPath, const std::string& remoteBranchName, const std::string& commitId) {
    std::string branchDir = Utils::join(remoteGitPath, ".branch");
    if (!Utils::isDirectory(branchDir)) Utils::createDirectories(branchDir);
    std::string remoteBranchFile = Utils::join(branchDir, remoteBranchName);
    Utils::writeContents(remoteBranchFile, commitId);
}

std::string Branch::getRemoteCommitId(const std::string& remoteGitPath, const std::string& remoteBranchName) {
    std::string branchDir = Utils::join(remoteGitPath, ".branch");
    std::string remoteBranchFile = Utils::join(branchDir, remoteBranchName);
    if (!Utils::exists(remoteBranchFile)) return "";
    return Utils::readContentsAsString(remoteBranchFile);
}
