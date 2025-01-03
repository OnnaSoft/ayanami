#ifndef STRINGS_HPP
#define STRINGS_HPP

#include "iostream"
#include <string_view>

std::string trim(const std::string_view& str);

std::string clean_null_terminated(const std::string_view& input);

#endif