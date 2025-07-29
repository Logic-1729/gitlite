#include "../include/Utils.h"
#include <cstdlib>
#include <iostream>
#include <sys/stat.h>
#include <cstring>

// SHA1 implementation
namespace SHA1 {
    void SHA::reset() {
        A = 0x67452301;
        B = 0xEFCDAB89;
        C = 0x98BADCFE;
        D = 0x10325476;
        E = 0xC3D2E1F0;
    }
    
    std::string SHA::padding(std::string message) {
        int originalLength = message.length();
        int newLength = ((originalLength + 8) + 63) / 64 * 64;
        std::string newMessage = message;
        newMessage.resize(newLength, 0);
        newMessage[originalLength] = static_cast<char>(0x80);
        int bitLength = originalLength * 8;
        for(int i = newLength - 1; i >= newLength - 8; i--) {
            newMessage[i] = bitLength % 256;
            bitLength /= 256;
        }
        return newMessage;
    }
    
    SHA::WORD SHA::charToWord(char ch) {
        return (BYTE)ch;
    }
    
    SHA::WORD SHA::shiftLeft(WORD x, int n) {
        return (x >> (32 - n)) | (x << n);
    }
    
    void SHA::getWord(std::string& message, int index) {
        for(int i = 0; i < 16; i++) {
            Word[i] = (charToWord(message[index + 4*i]) << 24) + 
                     (charToWord(message[index + 4*i + 1]) << 16) + 
                     (charToWord(message[index + 4*i + 2]) << 8) + 
                     charToWord(message[index + 4*i + 3]);
        }
        for(int i = 16; i < 80; i++) {
            Word[i] = shiftLeft(Word[i-3] ^ Word[i-8] ^ Word[i-14] ^ Word[i-16], 1);
        }
    }
    
    SHA::SHA() : Word(80) {
        reset();
    }
    
    SHA::WORD SHA::kt(int t) {
        if (t < 20)
            return 0x5a827999;
        else if (t < 40)
            return 0x6ed9eba1;
        else if (t < 60)
            return 0x8f1bbcdc;
        else
            return 0xca62c1d6;
    }
    
    SHA::WORD SHA::ft(int t, WORD B, WORD C, WORD D) {
        if (t < 20)
            return (B & C) | ((~B) & D);
        else if (t < 40)
            return B ^ C ^ D;
        else if (t < 60)
            return (B & C) | (B & D) | (C & D);
        else
            return B ^ C ^ D;
    }
    
    std::string SHA::sha(std::string message) {
        reset();
        message = padding(message);
        int byteLength = message.length();
        for(int i = 0; i < byteLength; i += 64) {
            getWord(message, i);
            WORD a = A, b = B, c = C, d = D, e = E;
            for(int j = 0; j < 80; j++) {
                WORD temp = shiftLeft(a, 5) + ft(j, b, c, d) + e + kt(j) + Word[j];
                e = d;
                d = c;
                c = shiftLeft(b, 30);
                b = a;
                a = temp;
            }
            A += a;
            B += b;
            C += c;
            D += d;
            E += e;
        }
        std::stringstream ss;
        ss << std::hex;
        ss << std::setw(8) << std::setfill('0') << A;
        ss << std::setw(8) << std::setfill('0') << B;
        ss << std::setw(8) << std::setfill('0') << C;
        ss << std::setw(8) << std::setfill('0') << D;
        ss << std::setw(8) << std::setfill('0') << E;
        return ss.str();
    }
    
    SHA sha;
    
    std::string sha1(std::string message) {
        return sha.sha(message);
    }
    
    std::string sha1(std::string s1, std::string s2) {
        return sha1(s1 + s2);
    }
    
    std::string sha1(std::string s1, std::string s2, std::string s3, std::string s4) {
        return sha1(s1 + s2 + s3 + s4);
    }
}

std::string Utils::sha1(const std::string& s1) {
    return SHA1::sha1(s1);
}

std::string Utils::sha1(const std::string& s1, const std::string& s2) {
    return SHA1::sha1(s1, s2);
}

std::string Utils::sha1(const std::string& s1, const std::string& s2, 
                       const std::string& s3, const std::string& s4) {
    return SHA1::sha1(s1, s2, s3, s4);
}

std::string Utils::sha1(const std::vector<unsigned char>& data) {
    std::string str(data.begin(), data.end());
    return SHA1::sha1(str);
}

bool Utils::restrictedDelete(const std::string& filepath) {
    // Extract parent directory
    size_t pos = filepath.find_last_of("/\\");
    std::string parentDir = (pos == std::string::npos) ? "." : filepath.substr(0, pos);
    std::string gitletDir = parentDir + "/.gitlet";
    
    if (!isDirectory(gitletDir)) {
        throw std::invalid_argument("not .gitlet working directory");
    }
    
    if (isFile(filepath)) {
        return remove(filepath.c_str()) == 0;
    }
    return false;
}

std::vector<unsigned char> Utils::readContents(const std::string& filepath) {
    if (!isFile(filepath)) {
        throw std::invalid_argument("must be a normal file");
    }
    
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        throw std::invalid_argument("cannot open file");
    }
    
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<unsigned char> contents(size);
    file.read(reinterpret_cast<char*>(contents.data()), size);
    
    return contents;
}

std::string Utils::readContentsAsString(const std::string& filepath) {
    auto contents = readContents(filepath);
    return std::string(contents.begin(), contents.end());
}

void Utils::writeContents(const std::string& filepath, const std::string& content) {
    // Create parent directories if needed
    size_t pos = filepath.find_last_of("/\\");
    if (pos != std::string::npos) {
        std::string parentDir = filepath.substr(0, pos);
        createDirectories(parentDir);
    }
    
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        throw std::invalid_argument("cannot create file");
    }
    
    file.write(content.c_str(), content.size());
}

void Utils::writeContents(const std::string& filepath, const std::vector<unsigned char>& content) {
    // Create parent directories if needed
    size_t pos = filepath.find_last_of("/\\");
    if (pos != std::string::npos) {
        std::string parentDir = filepath.substr(0, pos);
        createDirectories(parentDir);
    }
    
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        throw std::invalid_argument("cannot create file");
    }
    
    file.write(reinterpret_cast<const char*>(content.data()), content.size());
}

std::vector<std::string> Utils::plainFilenamesIn(const std::string& dirPath) {
    std::vector<std::string> files;
    
    DIR* dir = opendir(dirPath.c_str());
    if (dir == nullptr) {
        return files;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_REG) { // Regular file
            files.push_back(std::string(entry->d_name));
        }
    }
    
    closedir(dir);
    std::sort(files.begin(), files.end());
    return files;
}

std::string Utils::join(const std::string& first, const std::string& second) {
    if (first.empty()) return second;
    if (second.empty()) return first;
    
    if (first.back() == '/' || first.back() == '\\') {
        return first + second;
    }
    return first + "/" + second;
}

std::string Utils::join(const std::string& first, const std::string& second, const std::string& third) {
    return join(join(first, second), third);
}

std::vector<unsigned char> Utils::serialize(const std::string& obj) {
    return std::vector<unsigned char>(obj.begin(), obj.end());
}

void Utils::message(const std::string& msg) {
    std::cout << msg << std::endl;
}

void Utils::exitWithMessage(const std::string& msg) {
    message(msg);
    std::exit(0);
}

bool Utils::exists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

bool Utils::isFile(const std::string& path) {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) != 0) {
        return false;
    }
    return S_ISREG(buffer.st_mode);
}

bool Utils::isDirectory(const std::string& path) {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) != 0) {
        return false;
    }
    return S_ISDIR(buffer.st_mode);
}

bool Utils::createDirectories(const std::string& path) {
    if (path.empty()) return true;
    if (isDirectory(path)) return true;
    
    size_t pos = path.find_last_of("/\\");
    if (pos != std::string::npos) {
        std::string parent = path.substr(0, pos);
        if (!createDirectories(parent)) {
            return false;
        }
    }
    
    return mkdir(path.c_str(), 0755) == 0 || isDirectory(path);
}
