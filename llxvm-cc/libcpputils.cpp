/*
* Copyright (C) 2013 Tokyo System House Co.,Ltd.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation; either version 3,
* or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; see the file COPYING.LIB.  If
* not, write to the Free Software Foundation, 51 Franklin Street, Fifth Floor
* Boston, MA 02110-1301 USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdbool.h>
#include <math.h>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <cstring>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <regex>

#include "libcpputils.h"


int strim(char * buf)
{
	int len = (int)strlen(buf);
	if (len == 0) return 0;
	while (len > 0) {
		if (buf[len - 1] != '\n' && buf[len - 1] != '\r' &&
			buf[len - 1] != ' ' && buf[len - 1] != '\t')
		{
			break;
		}
		buf[--len] = 0;
	}
	if (len == 0) return 0;
	if (*buf == ' ' || *buf == '\t') {
		char * p = buf;
		char * q = buf + 1;
		while (*q == ' ' || *q == '\t') {
			++q;
			--len;
		}
		while ((*p++ = *q++) != 0);
	}
	return len;
}

char *safe_strdup(char * s)
{
	return (s != NULL) ? strdup(s) : NULL;
}

// trim from start (in place)
void ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
	}));
}

// trim from end (in place)
void rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
	}).base(), s.end());
}

// trim from both ends (in place)
void trim(std::string &s) {
	ltrim(s);
	rtrim(s);
}

// trim from start (copying)
std::string ltrim_copy(std::string s) {
	ltrim(s);
	return s;
}

// trim from end (copying)
std::string rtrim_copy(std::string s) {
	rtrim(s);
	return s;
}

// trim from both ends (copying)
std::string trim_copy(std::string s) {
	trim(s);
	return s;
}

bool starts_with(std::string s1, std::string s2)
{
	return s1.substr(0, s2.length()) == s2;
}

bool ends_with(std::string const &fullString, std::string const &ending)
{
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	else {
		return false;
	}
}

std::string lpad(const std::string &s, int len)
{
	std::string s1 = s;
	if (s1.size() >= len)
		return s1;

	for (int i = 0; i < len - s1.size(); i++)
		s1 = ' ' + s1;

	return s1;
}

std::string rpad(const std::string &s, int len)
{
	std::string s1 = s;
	if (s1.size() >= len)
		return s1;

	for (int i = 0; i < len - s1.size(); i++)
		s1 = s1 + ' ';

	return s1;
}

std::string string_chop(const std::string &s, int len)
{
	std::string s1 = s;
	if (s1.size() < len)
		len = s1.size();

	int new_len = s1.size() - len;
	return s1.substr(0, new_len);
}

bool string_contains(const std::string &s1, const std::string &s2)
{
	return s1.find(s2) != std::string::npos;
}

std::string string_replace(std::string subject, const std::string &search, const std::string &replace)
{
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return subject;
}

std::vector<std::string> file_read_all_lines(const std::string &filename)
{
	std::vector<std::string> res;

	std::filesystem::path filepath(filename);

	if (!std::filesystem::exists(filepath)) {
		return res;
	}

	std::ifstream ifs(filepath);
	
	std::string line;
	while (std::getline(ifs, line)) {
		res.push_back(line);
	}

	ifs.close();

	return res;
}

bool file_write_all_lines(const std::string &filename, const std::vector<std::string> &lines)
{
	std::filesystem::path filepath(filename);

	std::ofstream ofs(filepath);

	for (std::string line : lines) {
		ofs << line << std::endl;
	}

	ofs.close();

	return true;
}

bool file_exists(const std::string &filename)
{
	std::filesystem::path fp(filename);
	return std::filesystem::exists(fp);
}

std::string filename_change_ext(const std::string &filename, const std::string &ext)
{
	std::filesystem::path fp(filename);
	return fp.replace_extension(ext).string();
}

std::string filename_get_name(const std::string &filename)
{
	std::filesystem::path fp(filename);
	return fp.filename().string();
}

std::string filename_absolute_path(const std::filesystem::path &filepath)
{
	return std::filesystem::absolute(filepath).string();
}

std::string filename_clean_path(const std::string &filepath)
{
	std::string s = filepath;
	std::replace(s.begin(), s.end(), '\\', '/');
	return s;
}

std::string filename_absolute_path(const std::string &filename)
{
	std::filesystem::path fp(filename);
	return filename_absolute_path(fp);
}

bool caseInsensitiveStringCompare(const std::string& str1, const std::string& str2) {
	if (str1.size() != str2.size()) {
		return false;
	}
	for (std::string::const_iterator c1 = str1.begin(), c2 = str2.begin(); c1 != str1.end(); ++c1, ++c2) {
		if (tolower(*c1) != tolower(*c2)) {
			return false;
		}
	}
	return true;
}

std::vector<std::string> split_with_quotes(const std::string& in_s)
{
    std::istringstream iss(in_s);
    std::vector<std::string> v;
    std::string s;

    while (iss >> std::quoted(s)) {
        v.push_back(s);
    }
    return v;
}

std::vector<std::string> string_split(const std::string str, const std::string regex_str)
{   
	std::regex regexz(regex_str);
	std::sregex_token_iterator token_iter(str.begin(), str.end(), regexz, -1);
	std::sregex_token_iterator end;
	std::vector<std::string> list;
	while (token_iter != end) {
		list.emplace_back(*token_iter++);
	}
	return list;
}

std::vector<std::string> string_split2(const std::string s, const char c)
{
	std::vector<std::string> res;
	std::string cur_string;

	for (int i = 0; i < s.size(); i++) {
		char cc = s.at(i);
		if (cc == c) {
			res.push_back(cur_string);
			cur_string.clear();
			continue;
		}

		cur_string += cc;
	}

	res.push_back(cur_string);

	return res;
}

std::string string_to_lower(const std::string &s)
{
	std::string data = std::string(s);
	std::transform(data.begin(), data.end(), data.begin(), ::tolower);
	return data;
}

std::string string_to_upper(const std::string &s)
{
	std::string data = std::string(s);
	std::transform(data.begin(), data.end(), data.begin(), ::toupper);
	return data;
}



bool file_is_writable(const std::string &filename)
{
	std::string dir_path = std::filesystem::path(filename).parent_path().string();
	auto fs = std::filesystem::status(filename);
	auto p = fs.permissions();
	return ((p & std::filesystem::perms::owner_write) != std::filesystem::perms::none) ||
		((p & std::filesystem::perms::group_write) != std::filesystem::perms::none);
}

std::string vector_join(const std::vector<std::string> &v, char sep)
{
	std::string s;

	for (std::vector< std::string>::const_iterator p = v.begin();
		p != v.end(); ++p) {
		s += *p;
		if (p != v.end() - 1)
			s += sep;
	}
	return s;
}

std::string vector_join(const std::vector<std::string> &v, std::string sep, bool ignore_empty)
{
	std::string s;

	for (std::vector< std::string>::const_iterator p = v.begin(); p != v.end(); ++p) {
        if (ignore_empty && *p == "")
            continue;
        
		s += *p;
		if (p != v.end() - 1)
			s += sep;
	}
	return s;
}
