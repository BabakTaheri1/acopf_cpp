#pragma once
#include <string>

namespace acopf {

std::string JoinPath(const std::string& a, const std::string& b);
std::string Stem(const std::string& path);
bool Exists(const std::string& path);
bool EnsureDir(const std::string& path);

std::string PrepareResultsDir(const std::string& case_name);

} // namespace acopf
