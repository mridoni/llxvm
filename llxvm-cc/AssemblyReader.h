#pragma once

#include <string>
#include <vector>

struct AssemblyInfo
{
	// only public + static
	std::string path;
	std::vector<std::string> methods;
	std::vector<std::string> fields;
};

class AssemblyReader
{
public:
	static AssemblyInfo *load(const std::string &name);

};

