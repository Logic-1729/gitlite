#ifndef HEAD_H
#define HEAD_H

#include <string>

class HEAD {
public:
    static void setBranchName(const std::string& branchName);
    static std::string getBranchName();
};

#endif // HEAD_H
