#pragma once

#include <string>
#include <memory>
#include <map>

extern "C" {
#include "zip.h"
}

#include "classreader.h"

class JarReader
{
public:
    JarReader();
    
    bool read(const std::string& jar_path);
    void setVerbose(bool value);   

    std::map<std::string, ClassInfo *> classes;

private:
    bool verbose = false;

    void processClass(zip_t *zip);

};

