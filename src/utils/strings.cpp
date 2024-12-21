#include "iostream"

std::string trim(const std::string_view& str) {
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) start++;

    auto end = str.end();
    do { end--; } while (std::distance(start, end) > 0 && std::isspace(*end));

    return std::string(start, end + 1);
}

std::string clean_null_terminated(const std::string_view& input) {
    if (size_t pos = input.find('\0'); pos != std::string::npos) {
        return std::string(input.substr(0, pos));
    }
    return std::string(input);
}
