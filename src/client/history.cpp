
#include <iostream>
#include <fstream>
#include <readline/history.h>

std::string get_history_file_path() {
    const char* home_dir = std::getenv("HOME");
    if (!home_dir) {
        std::cerr << "No se pudo obtener la variable HOME. Usando el directorio actual." << std::endl;
        return ".ayanami_history";
    }
    return std::string(home_dir) + "/.ayanami_history";
}

void load_history_from_file(const std::string& filename) {
    std::ifstream infile(filename);
    if (!infile) return;

    std::string line;
    while (std::getline(infile, line)) {
        if (!line.empty()) {
            add_history(line.c_str());
        }
    }
    infile.close();
}

void save_history_to_file(const std::string& filename) {
    write_history(filename.c_str());
}