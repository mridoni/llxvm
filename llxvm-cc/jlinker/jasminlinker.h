#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>

class ClassInfo;


class JasminLinker
{
public:
    JasminLinker();
    
    bool performLink(const std::string& input_file, const std::string& output_file, 
                     const std::vector<std::string> libs, const std::vector<std::string> libdirs);
    
    void setRecursiveScan(bool value);
    void setUseKrakatau(bool value);
    void setUseWideLdc(bool value);
    void setVerbose(bool value);
    void setDebugLevel(int value);

private: 
    std::string input_file;
    std::string output_file;

    bool recursive_scan = false;
    bool use_krakatau = false;
    bool wide_ldc = false;
    bool verbose = false;
    bool is_error = false;
    int debug_level = 1;
    int unresolved_instances_count = 0;
    std::set<std::string> unresolved_symbols;

    std::ifstream  in;
    std::ofstream  out;

    bool read_line(std::string &line);
    void out_write(const std::string &s);
    std::string safe_get(const std::map<std::string, std::string> &map, const std::string &key);

    std::map<std::string, ClassInfo *> lib_classes;  
    std::map<std::string, std::string> method_map;
    std::map<std::string, std::string> field_map;

    std::vector<std::string> extern_fields;
    std::vector<std::string> extern_methods;

    void scan_dir(const std::string& libdir);
    bool scan_file(const std::string &filename);
    bool scan_jar(const std::string& jarpath);
    bool scan_jasmin(const std::string& jpath);

    void buildMethodMap();
    void buildFieldMap();

    void readExtern();
    void linkStatic();
    void printInvokeStatic(std::string methodName);
    void printGetStatic(std::string fieldName);
    void printClassForMethod(std::string methodName);
};
