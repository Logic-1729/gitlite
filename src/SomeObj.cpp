#include "../include/SomeObj.h"
#include "../include/Repository.h"
#include "../include/StagingArea.h"
#include "../include/Commit.h"
#include "../include/Blob.h"
#include "../include/Branch.h"
#include "../include/HEAD.h"
#include "../include/Remote.h"
#include "../include/Utils.h"
#include <iostream>
#include <algorithm>
#include <queue>
#include <sstream>

void SomeObj::init() {
    std::string gitliteDir = Repository::getGitliteDir();
    if (Utils::isDirectory(gitliteDir)) Utils::exitWithMessage("A Gitlite version-control system already exists in the current directory.");
    Utils::createDirectories(gitliteDir);
    Utils::createDirectories(Repository::getObjectsDir());
    Utils::createDirectories(Repository::getCommitsDir());
    Utils::createDirectories(Repository::getBranchDir());
    
    StagingArea stagingArea;
    stagingArea.save();
    
    Commit initialCommit;
    initialCommit.save();
    
    Branch::setCommitId("master", initialCommit.getSHA1());
    HEAD::setBranchName("master");
}

void SomeObj::add(const std::string& fileName) {
    std::string filePath = Utils::join(Repository::getCWD(), fileName);
    StagingArea currentStagingArea = StagingArea::load();
    
    if (!Utils::exists(filePath) && currentStagingArea.getRemoveStage().find(fileName) == currentStagingArea.getRemoveStage().end()) Utils::exitWithMessage("File does not exist.");
    
    if (currentStagingArea.getRemoveStage().find(fileName) != currentStagingArea.getRemoveStage().end()) {
        currentStagingArea.getRemoveStage().erase(fileName);
        currentStagingArea.save();
        return;
    }
    
    Blob currentBlob(fileName);
    std::string currentSHA1 = currentBlob.getSHA1();
    Commit headCommit = Commit::load(Branch::getCommitId(HEAD::getBranchName()));
    
    // First, remove any existing entry for this file name
    for (auto it = currentStagingArea.getAddStage().begin(); it != currentStagingArea.getAddStage().end(); ++it) {
        if (it->second == fileName) {
            currentStagingArea.getAddStage().erase(it);
            break;
        }
    }
    
    if (headCommit.getBlobTree().find(currentSHA1) != headCommit.getBlobTree().end()) {
        // File exists in head commit with same content - don't add to staging area
        // The file has been removed from staging area above, so no further action needed
    } else {
        // File is different from head commit, add to staging area
        currentStagingArea.getAddStage()[currentSHA1] = fileName;
    }
    
    currentStagingArea.save();
    currentBlob.save();
}

void SomeObj::commit(const std::string& message) {
    StagingArea currentStagingArea = StagingArea::load();
    if (currentStagingArea.getAddStage().empty() && currentStagingArea.getRemoveStage().empty()) Utils::exitWithMessage("No changes added to the commit.");
    if (message.empty()) Utils::exitWithMessage("Please enter a commit message.");
    std::string headCommitId = Branch::getCommitId(HEAD::getBranchName());
    Commit currentCommit(message, headCommitId, "");
    currentCommit.save();
    Branch::setCommitId(HEAD::getBranchName(), currentCommit.getSHA1());
}

void SomeObj::mergeCommit(const std::string& message, const std::string& head, const std::string& another) {
    if (message.empty()) Utils::exitWithMessage("Please enter a commit message.");
    Commit currentCommit(message, head, another);
    currentCommit.save();
    Branch::setCommitId(HEAD::getBranchName(), currentCommit.getSHA1());
}

void SomeObj::rm(const std::string& fileName) {
    StagingArea currentStagingArea = StagingArea::load();
    Commit currentCommit = Commit::load(Branch::getCommitId(HEAD::getBranchName()));
    
    // Check if file is in staging area
    bool inAddStage = false;
    for (const auto& pair : currentStagingArea.getAddStage()) {
        if (pair.second == fileName) {
            currentStagingArea.getAddStage().erase(pair.first);
            inAddStage = true;
            break;
        }
    }
    
    // Check if file is tracked in current commit
    bool inCommit = false;
    for (const auto& pair : currentCommit.getBlobTree()) {
        if (pair.second == fileName) {
            currentStagingArea.getRemoveStage().insert(fileName);
            Utils::restrictedDelete(Utils::join(Repository::getCWD(), fileName));
            inCommit = true;
            break;
        }
    }
    
    if (!inAddStage && !inCommit) Utils::exitWithMessage("No reason to remove the file.");
    currentStagingArea.save();
}

void SomeObj::log() {
    Commit currentCommit;
    std::string branchName = HEAD::getBranchName();
    
    if (branchName.find("/") != std::string::npos) {
        std::string branchFile = branchNameToBranchFile(branchName);
        std::string commitId = Utils::readContentsAsString(branchFile);
        currentCommit = Commit::load(commitId);
    } else {
        currentCommit = Commit::load(Branch::getCommitId(branchName));
    }
    
    while (!currentCommit.getParent1().empty()) {
        if (currentCommit.getParent2().empty()) {
            std::cout << "===\n"
                      << "commit " << currentCommit.getSHA1() << "\n"
                      << "Date: " << currentCommit.getTimestamp() << "\n"
                      << currentCommit.getMessage() << "\n\n";
        } else {
            std::string shortId1 = currentCommit.getParent1().substr(0, 7);
            std::string shortId2 = currentCommit.getParent2().substr(0, 7);
            std::cout << "===\n"
                      << "commit " << currentCommit.getSHA1() << "\n"
                      << "Merge: " << shortId1 << " " << shortId2 << "\n"
                      << "Date: " << currentCommit.getTimestamp() << "\n"
                      << currentCommit.getMessage() << "\n\n";
        }
        currentCommit = Commit::load(currentCommit.getParent1());
    }
    
    std::cout << "===\n"
              << "commit " << currentCommit.getSHA1() << "\n"
              << "Date: " << currentCommit.getTimestamp() << "\n"
              << currentCommit.getMessage() << "\n\n";
}

void SomeObj::globalLog() {
    std::vector<std::string> commitList = Utils::plainFilenamesIn(Repository::getCommitsDir());
    for (const std::string& commitId : commitList) {
        Commit currentCommit = Commit::load(commitId);
        if (currentCommit.getParent2().empty()) {
            std::cout << "===\n"
                      << "commit " << commitId << "\n"
                      << "Date: " << currentCommit.getTimestamp() << "\n"
                      << currentCommit.getMessage() << "\n\n";
        } else {
            std::string shortId1 = currentCommit.getParent1().substr(0, 7);
            std::string shortId2 = currentCommit.getParent2().substr(0, 7);
            std::cout << "===\n"
                      << "commit " << commitId << "\n"
                      << "Merge: " << shortId1 << " " << shortId2 << "\n"
                      << "Date: " << currentCommit.getTimestamp() << "\n"
                      << currentCommit.getMessage() << "\n\n";
        }
    }
}

void SomeObj::find(const std::string& commitMessage) {
    std::vector<std::string> commitList = Utils::plainFilenamesIn(Repository::getCommitsDir());
    std::vector<std::string> matchingCommits;
    
    for (const std::string& commitId : commitList) {
        Commit currentCommit = Commit::load(commitId);
        if (currentCommit.getMessage() == commitMessage) matchingCommits.push_back(commitId);
    }
    
    if (matchingCommits.empty()) {
        Utils::exitWithMessage("Found no commit with that message.");
    } else {
        for (const std::string& commitId : matchingCommits) std::cout << commitId << std::endl;
    }
}

void SomeObj::status() {
    StagingArea stagingArea = StagingArea::load();
    std::map<std::string, std::string> addStage = stagingArea.getAddStage();
    std::set<std::string> removeStage = stagingArea.getRemoveStage();
    std::vector<std::string> branchNameList = Utils::plainFilenamesIn(Repository::getBranchDir());
    
    std::sort(branchNameList.begin(), branchNameList.end());
    std::vector<std::string> sortedAddStageName = sortMapNames(addStage);
    std::vector<std::string> sortedRemoveStageName = sortSetNames(removeStage);
    Commit headCommit = Commit::load(Branch::getCommitId(HEAD::getBranchName()));
    
    std::cout << "=== Branches ===\n";
    for (const std::string& branchName : branchNameList) {
        if (branchName == HEAD::getBranchName()) std::cout << "*";
        std::cout << branchName << "\n";
    }
    
    std::cout << "\n=== Staged Files ===\n";
    for (const std::string& stageName : sortedAddStageName) std::cout << stageName << "\n";
    std::cout << "\n=== Removed Files ===\n";
    for (const std::string& stageName : sortedRemoveStageName) std::cout << stageName << "\n";
    std::cout << "\n=== Modifications Not Staged For Commit ===\n";
    std::set<std::string> modifiedNameToPrint;
    std::vector<std::string> fileNameInCWD = Utils::plainFilenamesIn(Repository::getCWD());
    
    for (const std::string& fileName : fileNameInCWD) {
        std::string filePath = Utils::join(Repository::getCWD(), fileName);
        std::vector<unsigned char> fileContent = Utils::readContents(filePath);
        std::string currentFileHash = Utils::sha1(fileName + std::string(fileContent.begin(), fileContent.end()));
        std::string commitBlobHash = valueToKey(headCommit.getBlobTree(), fileName);
        std::string addStageFileHash = valueToKey(addStage, fileName);
        
        bool isExist = Utils::exists(filePath);
        bool isInAddStage = (valueToKey(addStage, fileName) != "");
        bool isInCommit = (valueToKey(headCommit.getBlobTree(), fileName) != "");
        bool differentInCommit = false;
        bool differentInAddStage = false;
        
        if (isExist && isInCommit && currentFileHash != commitBlobHash) differentInCommit = true;
        if (isExist && isInAddStage && addStageFileHash != currentFileHash) differentInAddStage = true;
        if (isExist && (differentInCommit || differentInAddStage)) modifiedNameToPrint.insert(fileName + " (modified)");
    }
    
    for (const auto& entry : headCommit.getBlobTree()) {
        std::string fileName = entry.second;
        std::string filePath = Utils::join(Repository::getCWD(), fileName);
        bool isNotExist = !Utils::exists(filePath);
        bool isNotInRemoveStage = (removeStage.find(fileName) == removeStage.end());
        bool isInAddStage = (valueToKey(addStage, fileName) != "");
        if (isNotExist && (isNotInRemoveStage || isInAddStage)) modifiedNameToPrint.insert(fileName + " (deleted)");
    }
    
    for (const std::string& name : modifiedNameToPrint) std::cout << name << "\n";
    std::cout << "\n=== Untracked Files ===\n";
    std::vector<std::string> fileList2 = Utils::plainFilenamesIn(Repository::getCWD());
    std::set<std::string> untrackedNameToPrint;
    
    for (const std::string& fileName : fileList2) {
        bool isNotInHead = (valueToKey(headCommit.getBlobTree(), fileName) == "");
        bool isNotInAddStage = (valueToKey(addStage, fileName) == "");
        bool isInRemoveStage = (removeStage.find(fileName) != removeStage.end());
        
        if ((isNotInHead && isNotInAddStage) || isInRemoveStage) untrackedNameToPrint.insert(fileName);
    }
    
    for (const std::string& name : untrackedNameToPrint) std::cout << name << "\n";
}

void SomeObj::checkoutFile(const std::string& fileName) {
    checkoutFileInCommit(Branch::getCommitId(HEAD::getBranchName()), fileName);
}

void SomeObj::checkoutFileInCommit(const std::string& commitId, const std::string& fileName) {
    Commit targetCommit = Commit::load(commitId);
    if (!targetCommit.isValid()) Utils::exitWithMessage("No commit with that id exists.");
    
    // Check if file exists in the commit using containsValue logic
    bool fileExistsInCommit = false;
    for (const auto& entry : targetCommit.getBlobTree()) {
        if (entry.second == fileName) {
            fileExistsInCommit = true;
            break;
        }
    }
    
    if (!fileExistsInCommit) Utils::exitWithMessage("File does not exist in that commit.");
    
    std::string fileSHA1 = valueToKey(targetCommit.getBlobTree(), fileName);
    std::string blobPath = Utils::join(Repository::getObjectsDir(), fileSHA1);
    std::string blobData = Utils::readContentsAsString(blobPath);
    Blob currentBlob = Blob::deserialize(blobData);
    
    std::string outputPath = Utils::join(Repository::getCWD(), currentBlob.getName());
    Utils::writeContents(outputPath, currentBlob.getContent());
}

void SomeObj::checkoutBranch(const std::string& branchName) {
    if (branchName.find("/") != std::string::npos) {
        std::string branchFile = branchNameToBranchFile(branchName);
        std::string commitId = Utils::readContentsAsString(branchFile);
        Commit branchCommit = Commit::load(commitId);
        checkoutCommit(branchCommit);
        HEAD::setBranchName(branchName);
        return;
    }
    
    if (branchName == HEAD::getBranchName()) Utils::exitWithMessage("No need to checkout the current branch.");
    
    
    std::vector<std::string> branchNameList = Utils::plainFilenamesIn(Repository::getBranchDir());
    bool branchExists = std::find(branchNameList.begin(), branchNameList.end(), branchName) != branchNameList.end();
    
    if (!branchExists) Utils::exitWithMessage("No such branch exists.");
    
    Commit branchCommit = Commit::load(Branch::getCommitId(branchName));
    checkoutCommit(branchCommit);
    HEAD::setBranchName(branchName);
}

void SomeObj::checkoutCommit(const Commit& commit) {
    std::vector<std::string> fileList = Utils::plainFilenamesIn(Repository::getCWD());
    StagingArea stagingArea = StagingArea::load();
    std::string headCommitId = Branch::getCommitId(HEAD::getBranchName());
    Commit headCommit = Commit::load(headCommitId);
    
    // Check for untracked files that would be overwritten
    for (const std::string& fileName : fileList) {
        bool isNotInHead = true;
        bool isNotInAddStage = true;
        bool isInCurrentCommit = false;
        
        // Check if file is in head commit (containsValue logic)
        for (const auto& entry : headCommit.getBlobTree()) {
            if (entry.second == fileName) {
                isNotInHead = false;
                break;
            }
        }
        
        // Check if file is in add stage (containsValue logic) 
        for (const auto& entry : stagingArea.getAddStage()) {
            if (entry.second == fileName) {
                isNotInAddStage = false;
                break;
            }
        }
        
        // Check if file is in target commit (containsValue logic)
        for (const auto& entry : commit.getBlobTree()) {
            if (entry.second == fileName) {
                isInCurrentCommit = true;
                break;
            }
        }
        
        if (isNotInHead && isNotInAddStage && isInCurrentCommit) Utils::exitWithMessage("There is an untracked file in the way; delete it, or add and commit it first.");
        
    }
    
    if (commit.getSHA1() == headCommitId) return; // No changes needed
    
    // Remove files that are in head but not in target commit (using containsKey logic)
    for (const auto& entry : headCommit.getBlobTree()) {
        std::string fileSHA1 = entry.first;
        if (commit.getBlobTree().find(fileSHA1) == commit.getBlobTree().end()) {
            std::string blobPath = Utils::join(Repository::getObjectsDir(), fileSHA1);
            if (Utils::exists(blobPath)) {
                try {
                    std::string blobData = Utils::readContentsAsString(blobPath);
                    Blob currentBlob = Blob::deserialize(blobData);
                    std::string filePath = Utils::join(Repository::getCWD(), currentBlob.getName());
                    if (Utils::exists(filePath)) {
                        remove(filePath.c_str());
                    }
                } catch (const std::exception& e) {
                    // Skip this blob if there's an error reading it
                    continue;
                }
            }
        }
    }
    
    // Add/update files from target commit
    for (const auto& entry : commit.getBlobTree()) {
        std::string fileSHA1 = entry.first;
        std::string blobPath = Utils::join(Repository::getObjectsDir(), fileSHA1);
        if (Utils::exists(blobPath)) {
            std::ifstream file(blobPath);
            if (file.is_open()) {
                std::string blobData((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());
                file.close();
                
                Blob currentBlob = Blob::deserialize(blobData);
                
                std::string outputPath = Utils::join(Repository::getCWD(), currentBlob.getName());
                Utils::writeContents(outputPath, currentBlob.getContent());
            }
        }
    }
    
    // Clear staging area
    stagingArea.clear();
    stagingArea.save();
}

void SomeObj::branch(const std::string& branchName) {
    std::vector<std::string> branchNameList = Utils::plainFilenamesIn(Repository::getBranchDir());
    bool branchExists = std::find(branchNameList.begin(), branchNameList.end(), branchName) != branchNameList.end();
    
    if (branchExists) Utils::exitWithMessage("A branch with that name already exists.");
    
    Branch::setCommitId(branchName, Branch::getCommitId(HEAD::getBranchName()));
}

void SomeObj::rmBranch(const std::string& branchName) {
    std::vector<std::string> branchNameList = Utils::plainFilenamesIn(Repository::getBranchDir());
    bool branchExists = std::find(branchNameList.begin(), branchNameList.end(), branchName) != branchNameList.end();
    if (!branchExists) Utils::exitWithMessage("A branch with that name does not exist.");
    if (branchName == HEAD::getBranchName()) Utils::exitWithMessage("Cannot remove the current branch.");
    std::string branchFile = Utils::join(Repository::getBranchDir(), branchName);
    remove(branchFile.c_str());
}

void SomeObj::reset(const std::string& commitId) {
    Commit commit = Commit::load(commitId);
    if (!commit.isValid()) Utils::exitWithMessage("No commit with that id exists.");
    checkoutCommit(commit);
    Branch::setCommitId(HEAD::getBranchName(), commit.getSHA1());
}

// Helper methods
std::vector<std::string> SomeObj::sortMapNames(const std::map<std::string, std::string>& map) {
    std::vector<std::string> names;
    for (const auto& pair : map) names.push_back(pair.second);
    std::sort(names.begin(), names.end());
    return names;
}

std::vector<std::string> SomeObj::sortSetNames(const std::set<std::string>& set) {
    std::vector<std::string> names(set.begin(), set.end());
    std::sort(names.begin(), names.end());
    return names;
}

std::string SomeObj::valueToKey(const std::map<std::string, std::string>& map, const std::string& name) {
    for (const auto& pair : map) {
        if (pair.second == name) return pair.first;
    }
    return "";
}

std::string SomeObj::findAncestor(const std::string& headCommitId, const std::string& targetCommitId) {
    std::queue<std::string> headCommitQueue;
    std::queue<std::string> targetCommitQueue;
    std::set<std::string> booked;
    
    headCommitQueue.push(headCommitId);
    targetCommitQueue.push(targetCommitId);
    
    while (!headCommitQueue.empty()) {
        std::string currentCommitId = headCommitQueue.front();
        headCommitQueue.pop();
        
        if (currentCommitId == targetCommitId) return currentCommitId;
        booked.insert(currentCommitId);
        Commit cur = Commit::load(currentCommitId);
        if (!cur.getParent1().empty()) headCommitQueue.push(cur.getParent1());
        if (!cur.getParent2().empty()) headCommitQueue.push(cur.getParent2());
    }
    
    while (!targetCommitQueue.empty()) {
        std::string currentCommitId = targetCommitQueue.front();
        targetCommitQueue.pop();
        if (booked.find(currentCommitId) != booked.end()) return currentCommitId;
        Commit cur = Commit::load(currentCommitId);
        if (!cur.getParent1().empty()) targetCommitQueue.push(cur.getParent1());
        if (!cur.getParent2().empty()) targetCommitQueue.push(cur.getParent2());
    }
    
    return "";
}

void SomeObj::ensureRemoteDirExists() {
    std::string remoteDir = Repository::getRemoteDir();
    if (!Utils::isDirectory(remoteDir)) Utils::createDirectories(remoteDir);
}

// Simplified remote operations (basic implementation)
void SomeObj::addRemote(const std::string& remoteName, const std::string& remotePath) {
    ensureRemoteDirExists();
    if (!Remote::getRemotePath(remoteName).empty()) Utils::exitWithMessage("A remote with that name already exists.");
    std::string normalizedPath = remotePath;
    std::replace(normalizedPath.begin(), normalizedPath.end(), '/', '/');
    Remote::addRemotePath(remoteName, normalizedPath);
}

void SomeObj::rmRemote(const std::string& remoteName) {
    ensureRemoteDirExists();
    if (Remote::getRemotePath(remoteName).empty()) Utils::exitWithMessage("A remote with that name does not exist.");
    Remote::removeRemotePath(remoteName);
}

// Simplified implementations for push, fetch, pull
void SomeObj::push(const std::string& remoteName, const std::string& remoteBranchName) {
    ensureRemoteDirExists();
    std::string remoteGitPath = Remote::getRemotePath(remoteName);
    if (!Utils::exists(remoteGitPath)) Utils::exitWithMessage("Remote directory not found.");
    std::string localBranchName = HEAD::getBranchName();
    std::vector<std::string> commitHistory;
    std::string localCommitId = Branch::getCommitId(localBranchName);
    std::string remoteCommitId = Branch::getRemoteCommitId(remoteGitPath, remoteBranchName);
    
    bool isHistory = false;
    
    Commit commit = Commit::load(localCommitId);
    while (!commit.getParent1().empty()) {
        commitHistory.push_back(commit.getSHA1());
        if (commit.getSHA1() == remoteCommitId) {
            isHistory = true;
            break;
        }
        commit = Commit::load(commit.getParent1());
    }
    
    if (!isHistory) Utils::exitWithMessage("Please pull down remote changes before pushing.");
    
    for (const std::string& commitId : commitHistory) {
        commit = Commit::load(commitId);
        for (const auto& entry : commit.getBlobTree()) {
            Blob newBlob(entry.second);
            newBlob.saveOnRemotePath(remoteGitPath);
        }
        commit.saveOnRemotePath(remoteGitPath);
    }
    
    Branch::setRemoteCommitId(remoteGitPath, remoteBranchName, localCommitId);
}

void SomeObj::fetch(const std::string& remoteName, const std::string& remoteBranchName) {
    ensureRemoteDirExists();
    std::string remoteGitPath = Remote::getRemotePath(remoteName);
    
    if (!Utils::exists(remoteGitPath)) Utils::exitWithMessage("Remote directory not found.");
    if (Branch::getRemoteCommitId(remoteGitPath, remoteBranchName).empty()) Utils::exitWithMessage("That remote does not have that branch.");
    std::string localBranchName = remoteName + "/" + remoteBranchName;
    // On Linux, keep forward slash as separator
    
    std::string remoteCommitId = Branch::getRemoteCommitId(remoteGitPath, remoteBranchName);
    std::string localCommitId = Branch::getCommitId(localBranchName);
    std::vector<std::string> remoteCommitHistory;
    
    Commit commit = Commit::remoteLoad(remoteGitPath, remoteCommitId);
    while (!commit.getParent1().empty()) {
        remoteCommitHistory.push_back(commit.getSHA1());
        if (localCommitId == commit.getSHA1()) break;
        commit = Commit::remoteLoad(remoteGitPath, commit.getParent1());
    }
    if (remoteCommitHistory.empty()) Utils::exitWithMessage("Please pull down remote changes before pushing.");
    Branch::setCommitId2(remoteName, remoteBranchName, remoteCommitId);
    for (const std::string& commitId : remoteCommitHistory) {
        commit = Commit::remoteLoad(remoteGitPath, commitId);
        for (const auto& entry : commit.getBlobTree()) Blob::copyFromRemote(remoteGitPath, entry.first);
        commit.save();
    }
}

void SomeObj::pull(const std::string& remoteName, const std::string& remoteBranchName) {
    fetch(remoteName, remoteBranchName);
    std::string localBranchName = remoteName + "/" + remoteBranchName;
    merge(localBranchName);
}

// Merge implementation
void SomeObj::merge(const std::string& branchName) {
    StagingArea currentStage = StagingArea::load();
    std::string branchCommitId;
    
    if (branchName.find("/") != std::string::npos) {
        if (!currentStage.getAddStage().empty() || !currentStage.getRemoveStage().empty()) Utils::exitWithMessage("You have uncommitted changes.");
        std::string branchFile = branchNameToBranchFile(branchName);
        std::string remoteBranchCommitId = Utils::readContentsAsString(branchFile);
        Commit remoteBranchCommit = Commit::load(remoteBranchCommitId);
        if (!remoteBranchCommit.isValid()) Utils::exitWithMessage("A branch with that name does not exist.");
        if (branchName == HEAD::getBranchName()) Utils::exitWithMessage("Cannot merge a branch with itself.");
        branchCommitId = remoteBranchCommit.getSHA1();
    } else {
        if (!currentStage.getAddStage().empty() || !currentStage.getRemoveStage().empty()) Utils::exitWithMessage("You have uncommitted changes.");
        std::vector<std::string> branchNameList = Utils::plainFilenamesIn(Repository::getBranchDir());
        bool branchExists = std::find(branchNameList.begin(), branchNameList.end(), branchName) != branchNameList.end();
        if (!branchExists) Utils::exitWithMessage("A branch with that name does not exist.");
        if (branchName == HEAD::getBranchName()) Utils::exitWithMessage("Cannot merge a branch with itself.");
        branchCommitId = Branch::getCommitId(branchName);
    }
    
    std::vector<std::string> fileList2 = Utils::plainFilenamesIn(Repository::getCWD());
    Commit headCommit = Commit::load(Branch::getCommitId(HEAD::getBranchName()));
    for (const std::string& fileName : fileList2) {
        bool isNotInHead = true;
        bool isNotInAddStage = true;
        
        // Check if file is in head commit (containsValue logic)
        for (const auto& entry : headCommit.getBlobTree()) {
            if (entry.second == fileName) {
                isNotInHead = false;
                break;
            }
        }
        
        // Check if file is in add stage (containsValue logic)
        for (const auto& entry : currentStage.getAddStage()) {
            if (entry.second == fileName) {
                isNotInAddStage = false;
                break;
            }
        }
        
        if (isNotInHead && isNotInAddStage) Utils::exitWithMessage("There is an untracked file in the way; delete it, or add and commit it first.");
    }
    
    std::string headCommitId = Branch::getCommitId(HEAD::getBranchName());
    std::string splitPointCommitId = findAncestor(headCommitId, branchCommitId);
    
    if (splitPointCommitId == branchCommitId) Utils::exitWithMessage("Given branch is an ancestor of the current branch.");
    if (splitPointCommitId == headCommitId) {
        checkoutCommit(Commit::load(branchCommitId));
        Branch::setCommitId(HEAD::getBranchName(), branchCommitId);
        Utils::exitWithMessage("Current branch fast-forwarded.");
    }
    
    Commit branchCommit = Commit::load(branchCommitId);
    Commit splitPointCommit = Commit::load(splitPointCommitId);
    
    isConflict = false;
    skipFiles.clear();
    
    splitPointCheck(headCommit, branchCommit, splitPointCommit);
    notSplitCheck(splitPointCommit.getBlobTree(), headCommit.getBlobTree(), 
                  branchCommit.getBlobTree(), branchCommit, skipFiles);
    
    std::vector<std::string> fileList = Utils::plainFilenamesIn(Repository::getCWD());
    for (const std::string& fileName : fileList) {
        if (skipFiles.find(fileName) == skipFiles.end()) add(fileName);

    }
    
    std::string newMessage = "Merged " + branchName + " into " + HEAD::getBranchName() + ".";
    mergeCommit(newMessage, headCommitId, branchCommitId);
    
    if (isConflict) Utils::exitWithMessage("Encountered a merge conflict.");
}

void SomeObj::splitPointCheck(const Commit& headCommit, const Commit& branchCommit, const Commit& splitPointCommit) {
    const std::map<std::string, std::string>& splitPointTree = splitPointCommit.getBlobTree();
    const std::map<std::string, std::string>& headCommitTree = headCommit.getBlobTree();
    const std::map<std::string, std::string>& branchCommitTree = branchCommit.getBlobTree();
    
    for (const auto& entry : splitPointTree) {
        std::string currentKey = entry.first;
        std::string currentName = entry.second;
        std::string headKey = valueToKey(headCommitTree, currentName);
        std::string branchKey = valueToKey(branchCommitTree, currentName);
        
        bool isInHeadCommit = headCommitTree.find(currentKey) != headCommitTree.end();
        bool isNotInHeadCommit = !isInHeadCommit;
        bool isInBranchCommit = branchCommitTree.find(currentKey) != branchCommitTree.end();
        
        // Check if name exists in commits (containsValue logic)
        bool nameIsInHeadCommit = false;
        bool nameIsInBranchCommit = false;
        
        for (const auto& headEntry : headCommitTree) {
            if (headEntry.second == currentName) {
                nameIsInHeadCommit = true;
                break;
            }
        }
        
        for (const auto& branchEntry : branchCommitTree) {
            if (branchEntry.second == currentName) {
                nameIsInBranchCommit = true;
                break;
            }
        }
        
        bool nameIsNotInHeadCommit = !nameIsInHeadCommit;
        bool nameIsNotInBranchCommit = !nameIsInBranchCommit;
        
        if (isInHeadCommit && isInBranchCommit) {
            continue;
        } else if (isInHeadCommit) { // unmodified in HEAD
            if (nameIsInBranchCommit) { // modified in other--1
                checkoutFileInCommit(branchCommit.getSHA1(), currentName);
            } else if (nameIsNotInBranchCommit) { // not present in other--6
                rm(currentName);
            }
        } else if (isInBranchCommit) { // unmodified in other
            if (nameIsInHeadCommit && isNotInHeadCommit) {
                // modified in HEAD--2
                checkoutFileInCommit(headCommit.getSHA1(), currentName);
            }
            // not present in HEAD--7
        } else if (nameIsInHeadCommit && nameIsInBranchCommit) {
            if (headKey != branchKey) { // in diff way--3b,3a不变
                std::string conflictContents = conflictFileContents(headKey, branchKey);
                std::string filePath = Utils::join(Repository::getCWD(), currentName);
                Utils::writeContents(filePath, conflictContents);
            }
        } else if (nameIsInHeadCommit && nameIsNotInBranchCommit) {
            std::string conflictContents = conflictFileContents(headKey, branchKey);
            std::string filePath = Utils::join(Repository::getCWD(), currentName);
            Utils::writeContents(filePath, conflictContents);
        } else if (nameIsNotInHeadCommit && nameIsNotInBranchCommit) {
            skipFiles.insert(currentName);
        }
    }
}

std::string SomeObj::conflictFileContents(const std::string& headBlobId, const std::string& branchBlobId) {
    isConflict = true;
    std::string currentContents;
    std::string mergedContents;
    
    if (headBlobId.empty()) {
        currentContents = "";
    } else {
        std::string blobPath = Utils::join(Repository::getObjectsDir(), headBlobId);
        if (Utils::exists(blobPath)) {
            std::string blobData = Utils::readContentsAsString(blobPath);
            Blob blob = Blob::deserialize(blobData);
            std::vector<unsigned char> content = blob.getContent();
            currentContents = std::string(content.begin(), content.end());
        }
    }
    
    if (branchBlobId.empty()) {
        mergedContents = "";
    } else {
        std::string blobPath = Utils::join(Repository::getObjectsDir(), branchBlobId);
        if (Utils::exists(blobPath)) {
            std::string blobData = Utils::readContentsAsString(blobPath);
            Blob blob = Blob::deserialize(blobData);
            std::vector<unsigned char> content = blob.getContent();
            mergedContents = std::string(content.begin(), content.end());
        }
    }
    
    return "<<<<<<< HEAD\n" + currentContents + "=======\n" + mergedContents + ">>>>>>>\n";
}

void SomeObj::notSplitCheck(const std::map<std::string, std::string>& splitPointTree,
                           const std::map<std::string, std::string>& headCommitTree,
                           const std::map<std::string, std::string>& branchCommitTree,
                           const Commit& branchCommit,
                           const std::set<std::string>& skipFiles) {
    
    for (const auto& entry : branchCommitTree) {
        std::string currentName = entry.second;
        std::string currentKey = entry.first;
        
        if (skipFiles.count(currentName) > 0) {
            continue;
        }
        
        bool isNotInHeadCommit = headCommitTree.find(currentKey) == headCommitTree.end();
        bool nameIsNotInSplitPoint = true;
        bool nameIsInHeadCommit = false;
        bool nameIsNotInHeadCommit = true;
        
        // Check if name exists in split point
        for (const auto& splitEntry : splitPointTree) {
            if (splitEntry.second == currentName) {
                nameIsNotInSplitPoint = false;
                break;
            }
        }
        
        // Check if name exists in head commit
        for (const auto& headEntry : headCommitTree) {
            if (headEntry.second == currentName) {
                nameIsInHeadCommit = true;
                nameIsNotInHeadCommit = false;
                break;
            }
        }
        
        std::string headKey;
        std::string branchKey = currentKey;
        
        // Find corresponding head key
        for (const auto& headEntry : headCommitTree) {
            if (headEntry.second == currentName) {
                headKey = headEntry.first;
                break;
            }
        }
        
        if (nameIsNotInSplitPoint && nameIsNotInHeadCommit) {
            checkoutFileInCommit(branchCommit.getSHA1(), entry.second);
        } else if (nameIsNotInSplitPoint && nameIsInHeadCommit && isNotInHeadCommit) {
            std::string conflictContents = conflictFileContents(headKey, branchKey);
            Utils::writeContents(Utils::join(Repository::getCWD(), currentName), conflictContents);
        }
    }
}

std::string SomeObj::branchNameToBranchFile(const std::string& branchName) {
    if (branchName.find("/") != std::string::npos) {
        size_t pos = branchName.find("/");
        std::string remoteName = branchName.substr(0, pos);
        std::string remoteBranchName = branchName.substr(pos + 1);
        return Utils::join(Repository::getBranchDir(), remoteName, remoteBranchName);
    } else {
        return Utils::join(Repository::getBranchDir(), branchName);
    }
}