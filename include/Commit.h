#ifndef COMMIT_H
#define COMMIT_H

#include <string>
#include <map>

class Commit {
private:
    std::string message;
    std::string timestamp;
    std::string parent1;
    std::string parent2;
    std::map<std::string, std::string> blobTree; // key is SHA1, value is filename
    bool valid; // Flag to indicate if this commit is valid

public:
    Commit(); // Initial commit constructor
    Commit(const std::string& message, const std::string& parent1, const std::string& parent2);
    
    // Getters
    std::string getMessage() const;
    std::string getTimestamp() const;
    std::string getParent1() const;
    std::string getParent2() const;
    std::map<std::string, std::string>& getBlobTree();
    const std::map<std::string, std::string>& getBlobTree() const;
    
    // File operations
    void save() const;
    void saveOnRemotePath(const std::string& remotePath) const;
    std::string getSHA1() const;
    
    // Check if commit is valid/loaded
    bool isValid() const;
    
    // Static load methods
    static Commit load(const std::string& commitId);
    static Commit remoteLoad(const std::string& remoteGitPath, const std::string& remoteCommitId);
    
    // Serialization
    std::string serialize() const;
    static Commit deserialize(const std::string& data);

private:
    void buildBlobTree();
    std::string getDate() const;
    std::string generateTimestamp() const;
};

#endif // COMMIT_H
