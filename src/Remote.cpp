#include "../include/Remote.h"
#include "../include/Repository.h"
#include "../include/Utils.h"

void Remote::addRemotePath(const std::string& remoteName, const std::string& remoteGitPath) {
    std::string remoteFile = Utils::join(Repository::getRemoteDir(), remoteName);
    Utils::writeContents(remoteFile, remoteGitPath);
}

void Remote::removeRemotePath(const std::string& remoteName) {
    std::string remoteFile = Utils::join(Repository::getRemoteDir(), remoteName);
    if (Utils::exists(remoteFile)) {
        remove(remoteFile.c_str());
    }
}

std::string Remote::getRemotePath(const std::string& remoteName) {
    std::string remoteFile = Utils::join(Repository::getRemoteDir(), remoteName);
    if (!Utils::exists(remoteFile)) {
        return "";
    }
    return Utils::readContentsAsString(remoteFile);
}
