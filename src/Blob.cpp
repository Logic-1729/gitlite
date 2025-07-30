#include "../include/Blob.h"
#include "../include/Repository.h"
#include "../include/Utils.h"
#include <sstream>
#include <fstream>

std::vector<unsigned char> Blob::getContent() const {
    return contents;
}

std::string Blob::getName() const {
    return name;
}

Blob::Blob(const std::string& name) : name(name) {
    contents = getContents();
}

std::string Blob::getSHA1() const {
    return Utils::sha1(name,std::string(contents.begin(), contents.end()));
}

std::vector<unsigned char> Blob::getContents() const {
    std::string filePath = Utils::join(Repository::getCWD(), name);
    return Utils::readContents(filePath);
}

void Blob::save() const {
    std::string objectPath = Utils::join(Repository::getObjectsDir(), getSHA1());
    std::string serialized = serialize();
    Utils::writeContents(objectPath, serialized);
}

void Blob::saveOnRemotePath(const std::string& remotePath) const {
    std::string objectsDir = Utils::join(remotePath, ".objects");
    if (!Utils::isDirectory(objectsDir)) Utils::createDirectories(objectsDir);
    std::string objectPath = Utils::join(objectsDir, getSHA1());
    std::string serialized = serialize();
    Utils::writeContents(objectPath, serialized);
}

void Blob::copyFromRemote(const std::string& remotePath, const std::string& sha1) {
    std::string currentFile = Utils::join(Utils::join(remotePath, ".objects"), sha1);
    std::string targetFile = Utils::join(Repository::getObjectsDir(), sha1);
    
    if (Utils::exists(currentFile)) {
        std::vector<unsigned char> content = Utils::readContents(currentFile);
        Utils::writeContents(targetFile, content);
    }
}

std::string Blob::serialize() const {
    std::stringstream ss;
    ss << name << "\n";
    ss << contents.size() << "\n";
    for (unsigned char c : contents) ss << static_cast<int>(c) << " ";
    return ss.str();
}

Blob Blob::deserialize(const std::string& data) {
    std::istringstream iss(data);
    std::string name;
    std::getline(iss, name);
    
    std::string sizeLine;
    std::getline(iss, sizeLine);
    size_t size = std::stoul(sizeLine);
    
    // 使用私有默认构造函数
    Blob blob;
    blob.name = name;
    blob.contents.clear();
    blob.contents.reserve(size);
    
    std::string contentLine;
    std::getline(iss, contentLine);
    std::istringstream contentStream(contentLine);
    
    int value;
    while (contentStream >> value && blob.contents.size() < size) blob.contents.push_back(static_cast<unsigned char>(value));
    return blob;
}
