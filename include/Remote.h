#ifndef REMOTE_H
#define REMOTE_H

#include <string>

class Remote {
public:
    static void addRemotePath(const std::string& remoteName, const std::string& remoteGitPath);
    static void removeRemotePath(const std::string& remoteName);
    static std::string getRemotePath(const std::string& remoteName);
};

#endif // REMOTE_H
