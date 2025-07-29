#include "../include/GitletException.h"

GitletException::GitletException() : message("") {}

GitletException::GitletException(const std::string& msg) : message(msg) {}

const char* GitletException::what() const noexcept {
    return message.c_str();
}
