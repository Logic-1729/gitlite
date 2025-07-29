#ifndef GITLET_EXCEPTION_H
#define GITLET_EXCEPTION_H

#include <exception>
#include <string>

class GitletException : public std::exception {
private:
    std::string message;

public:
    GitletException();
    GitletException(const std::string& msg);
    virtual const char* what() const noexcept override;
};

#endif // GITLET_EXCEPTION_H
