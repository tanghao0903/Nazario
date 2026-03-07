#pragma once

#include <string>
#include <unordered_map>

class Env {
public:
    bool load(const std::string& path);
    std::string get(const std::string& key, const std::string& default_value = "") const;
    double get_double(const std::string& key, double default_value) const;
    int get_int(const std::string& key, int default_value) const;

private:
    std::unordered_map<std::string, std::string> values_;
};
