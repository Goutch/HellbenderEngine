#pragma once

#include <string>

class Script {
    std::string file_path;
    std::string code;
public:
    Script(std::string file_path);
    const std::string& getFile();
};
