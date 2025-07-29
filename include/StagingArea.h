#ifndef STAGING_AREA_H
#define STAGING_AREA_H

#include <map>
#include <set>
#include <string>
#include <vector>

class StagingArea {
private:
    std::set<std::string> removeStage;
    std::map<std::string, std::string> addStage; // key is SHA1, value is filename

public:
    StagingArea();
    
    static StagingArea load();
    void save() const;
    
    std::set<std::string>& getRemoveStage();
    std::map<std::string, std::string>& getAddStage();
    
    void clear();
    void removeAddStageValue(const std::string& value);
    
    // Serialization methods
    std::string serialize() const;
    void deserialize(const std::string& data);
};

#endif // STAGING_AREA_H
