#ifndef BLOB_H
#define BLOB_H

#include <string>
#include <vector>

class Blob {
private:
    std::vector<unsigned char> contents;
    std::string name;
    
    // 私有构造函数，用于反序列化
    Blob() = default;

public:
    Blob(const std::string& name);
    
    std::string getSHA1() const;
    std::vector<unsigned char> getContent() const;
    std::string getName() const;
    
    void save() const;
    void saveOnRemotePath(const std::string& remotePath) const;
    
    static void copyFromRemote(const std::string& remotePath, const std::string& sha1);
    
    // Serialization methods
    std::string serialize() const;
    static Blob deserialize(const std::string& data);

private:
    std::vector<unsigned char> getContents() const;
};

#endif // BLOB_H
