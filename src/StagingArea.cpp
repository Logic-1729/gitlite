#include "../include/StagingArea.h"
#include "../include/Repository.h"
#include "../include/Utils.h"
#include <sstream>

StagingArea::StagingArea() {
    removeStage.clear();
    addStage.clear();
}

StagingArea StagingArea::load() {
    StagingArea staging;
    std::string stageFile = Repository::getStageFile();
    
    if (Utils::exists(stageFile)) {
        std::string data = Utils::readContentsAsString(stageFile);
        staging.deserialize(data);
    }
    
    return staging;
}

void StagingArea::save() const {
    std::string stageFile = Repository::getStageFile();
    std::string data = serialize();
    Utils::writeContents(stageFile, data);
}

std::set<std::string>& StagingArea::getRemoveStage() {
    return removeStage;
}

std::map<std::string, std::string>& StagingArea::getAddStage() {
    return addStage;
}

void StagingArea::clear() {
    addStage.clear();
    removeStage.clear();
}

void StagingArea::removeAddStageValue(const std::string& value) {
    for (auto it = addStage.begin(); it != addStage.end(); ++it) {
        if (it->second == value) {
            addStage.erase(it);
            break;
        }
    }
}

std::string StagingArea::serialize() const {
    std::stringstream ss;
    
    // Serialize addStage
    ss << addStage.size() << "\n";
    for (const auto& pair : addStage) {
        ss << pair.first << "\n" << pair.second << "\n";
    }
    
    // Serialize removeStage
    ss << removeStage.size() << "\n";
    for (const auto& item : removeStage) {
        ss << item << "\n";
    }
    
    return ss.str();
}

void StagingArea::deserialize(const std::string& data) {
    std::istringstream iss(data);
    std::string line;
    
    // Deserialize addStage
    if (std::getline(iss, line)) {
        int addStageSize = std::stoi(line);
        for (int i = 0; i < addStageSize; ++i) {
            std::string key, value;
            if (std::getline(iss, key) && std::getline(iss, value)) {
                addStage[key] = value;
            }
        }
    }
    
    // Deserialize removeStage
    if (std::getline(iss, line)) {
        int removeStageSize = std::stoi(line);
        for (int i = 0; i < removeStageSize; ++i) {
            std::string item;
            if (std::getline(iss, item)) {
                removeStage.insert(item);
            }
        }
    }
}
