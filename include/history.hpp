
#include <iostream>
#include <fstream>
#include <readline/history.h>

std::string get_history_file_path();

void load_history_from_file(const std::string& filename);

void save_history_to_file(const std::string& filename);