#include "../include/Commit.h"
#include "../include/Repository.h"
#include "../include/StagingArea.h"
#include "../include/Utils.h"
#include <sstream>
#include <ctime>
#include <iomanip>

Commit::Commit() {
    message = "initial commit";
    parent1 = "";
    parent2 = "";
    timestamp = getDate();
    blobTree.clear();
    valid = true;
}

Commit::Commit(const std::string& message, const std::string& parent1, const std::string& parent2)
    : message(message), parent1(parent1), parent2(parent2), valid(true) {
    timestamp = getDate();
    blobTree.clear();
    buildBlobTree();
}

std::string Commit::getMessage() const {
    return message;
}

std::string Commit::getTimestamp() const {
    return timestamp;
}

std::string Commit::getParent1() const {
    return parent1;
}

std::string Commit::getParent2() const {
    return parent2;
}

std::map<std::string, std::string>& Commit::getBlobTree() {
    return blobTree;
}

const std::map<std::string, std::string>& Commit::getBlobTree() const {
    return blobTree;
}

void Commit::buildBlobTree() {
    // Load parent commit if exists
    if (!parent1.empty()) {
        std::string parentFile = Utils::join(Repository::getCommitsDir(), parent1);
        if (Utils::exists(parentFile)) {
            Commit parentCommit = load(parent1);
            blobTree = parentCommit.getBlobTree();
        }
    }
    
    StagingArea currentStagingArea = StagingArea::load();
    
    // Remove files that are being replaced
    for (const auto& entry : currentStagingArea.getAddStage()) {
        std::string fileName = entry.second;
        // Remove any existing entry with same filename
        for (auto it = blobTree.begin(); it != blobTree.end(); ++it) {
            if (it->second == fileName) {
                blobTree.erase(it);
                break;
            }
        }
    }
    
    // Add new files from staging area
    for (const auto& entry : currentStagingArea.getAddStage()) {
        blobTree[entry.first] = entry.second;
    }
    
    // Remove files from remove stage
    for (const std::string& removeFileName : currentStagingArea.getRemoveStage()) {
        for (auto it = blobTree.begin(); it != blobTree.end(); ++it) {
            if (it->second == removeFileName) {
                blobTree.erase(it);
                break;
            }
        }
    }
    
    // Clear staging area
    StagingArea stagingArea;
    stagingArea.save();
}

std::string Commit::getDate() const {
    if (!timestamp.empty()) {
        return timestamp;
    }
    return generateTimestamp();
}

std::string Commit::generateTimestamp() const {
    std::time_t rawtime;
    
    if (parent1.empty()) {
        rawtime = 0; // Default date for initial commit
    } else {
        std::time(&rawtime);
    }
    
    std::tm* timeinfo = std::gmtime(&rawtime);
    std::ostringstream oss;
    
    // Format: "EEE MMM dd HH:mm:ss yyyy Z"
    const char* days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                           "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    
    oss << days[timeinfo->tm_wday] << " " 
        << months[timeinfo->tm_mon] << " "
        << std::setfill('0') << std::setw(2) << timeinfo->tm_mday << " "
        << std::setfill('0') << std::setw(2) << timeinfo->tm_hour << ":"
        << std::setfill('0') << std::setw(2) << timeinfo->tm_min << ":"
        << std::setfill('0') << std::setw(2) << timeinfo->tm_sec << " "
        << (1900 + timeinfo->tm_year) << " +0000";
    
    return oss.str();
}

void Commit::save() const {
    std::string commitPath = Utils::join(Repository::getCommitsDir(), getSHA1());
    std::string serialized = serialize();
    Utils::writeContents(commitPath, serialized);
}

void Commit::saveOnRemotePath(const std::string& remotePath) const {
    std::string commitsDir = Utils::join(remotePath, ".commits");
    if (!Utils::isDirectory(commitsDir)) {
        Utils::createDirectories(commitsDir);
    }
    std::string commitPath = Utils::join(commitsDir, getSHA1());
    std::string serialized = serialize();
    Utils::writeContents(commitPath, serialized);
}

std::string Commit::getSHA1() const {
    std::string serialized = serialize();
    return Utils::sha1(serialized);
}

bool Commit::isValid() const {
    return valid;
}

Commit Commit::load(const std::string& commitId) {
    if (commitId.empty()) {
        Commit invalidCommit;
        invalidCommit.valid = false;
        return invalidCommit;
    }
    
    std::string fullCommitId = commitId;
    
    // Handle short commit IDs
    if (commitId.length() < 40) {
        std::vector<std::string> commitIdList = Utils::plainFilenamesIn(Repository::getCommitsDir());
        for (const std::string& id : commitIdList) {
            if (id.find(commitId) == 0) { // starts with commitId
                fullCommitId = id;
                break;
            }
        }
        // If no match found and we still have a short ID, it doesn't exist
        if (fullCommitId == commitId && commitId.length() < 40) {
            Commit invalidCommit;
            invalidCommit.valid = false;
            return invalidCommit;
        }
    }
    
    std::string commitFile = Utils::join(Repository::getCommitsDir(), fullCommitId);
    if (!Utils::exists(commitFile)) {
        Commit invalidCommit;
        invalidCommit.valid = false;
        return invalidCommit;
    }
    
    std::string data = Utils::readContentsAsString(commitFile);
    return deserialize(data);
}

Commit Commit::remoteLoad(const std::string& remoteGitPath, const std::string& remoteCommitId) {
    std::string fullCommitId = remoteCommitId;
    std::string branchFile = Utils::join(remoteGitPath, ".commits");
    
    // Handle short commit IDs
    if (remoteCommitId.length() < 40) {
        std::vector<std::string> commitIdList = Utils::plainFilenamesIn(branchFile);
        for (const std::string& id : commitIdList) {
            if (id.find(remoteCommitId) == 0) {
                fullCommitId = id;
                break;
            }
        }
    }
    
    std::string commitFile = Utils::join(branchFile, fullCommitId);
    if (!Utils::exists(commitFile)) {
        return Commit(); // Return empty commit if not found
    }
    
    std::string data = Utils::readContentsAsString(commitFile);
    return deserialize(data);
}

std::string Commit::serialize() const {
    std::ostringstream oss;
    oss << message << "\n";
    oss << timestamp << "\n";
    oss << parent1 << "\n";
    oss << parent2 << "\n";
    oss << blobTree.size() << "\n";
    
    for (const auto& pair : blobTree) {
        oss << pair.first << "\n" << pair.second << "\n";
    }
    
    return oss.str();
}

Commit Commit::deserialize(const std::string& data) {
    if (data.empty()) {
        Commit invalidCommit;
        invalidCommit.valid = false;
        return invalidCommit;
    }
    
    std::istringstream iss(data);
    Commit commit;
    commit.valid = true; // Mark as valid since we're deserializing real data
    
    std::getline(iss, commit.message);
    std::getline(iss, commit.timestamp);
    std::getline(iss, commit.parent1);
    std::getline(iss, commit.parent2);
    
    std::string blobTreeSizeStr;
    std::getline(iss, blobTreeSizeStr);
    if (blobTreeSizeStr.empty()) {
        return commit; // Return commit even if blob tree size is empty
    }
    
    size_t blobTreeSize = std::stoul(blobTreeSizeStr);
    
    commit.blobTree.clear();
    for (size_t i = 0; i < blobTreeSize; ++i) {
        std::string key, value;
        if (std::getline(iss, key) && std::getline(iss, value)) {
            commit.blobTree[key] = value;
        }
    }
    
    return commit;
}
