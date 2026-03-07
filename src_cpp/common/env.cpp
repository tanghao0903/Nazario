#include "common/env.hpp"

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <sstream>

namespace {
std::string trim(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
        ++start;
    }
    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
        --end;
    }
    return s.substr(start, end - start);
}
}

bool Env::load(const std::string& path) {
    std::ifstream input(path);
    if (!input.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        const auto pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }
        const std::string key = trim(line.substr(0, pos));
        const std::string value = trim(line.substr(pos + 1));
        if (!key.empty()) {
            values_[key] = value;
        }
    }
    return true;
}

std::string Env::get(const std::string& key, const std::string& default_value) const {
    const auto it = values_.find(key);
    if (it != values_.end()) {
        return it->second;
    }
    if (const char* from_process = std::getenv(key.c_str())) {
        return std::string(from_process);
    }
    return default_value;
}

double Env::get_double(const std::string& key, double default_value) const {
    const std::string value = get(key, "");
    if (value.empty()) {
        return default_value;
    }
    try {
        return std::stod(value);
    } catch (...) {
        return default_value;
    }
}

int Env::get_int(const std::string& key, int default_value) const {
    const std::string value = get(key, "");
    if (value.empty()) {
        return default_value;
    }
    try {
        return std::stoi(value);
    } catch (...) {
        return default_value;
    }
}
