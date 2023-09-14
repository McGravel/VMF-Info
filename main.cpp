#include "boost/algorithm/string.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

enum class Tokens {
    None,
    Brace_Open,
    Brace_Close,
    Visgroup_Block,
    Visgroup_Single,
    Solid,
    Entity,
    Side,
};

struct Visgroup {
    std::string_view name{};
    size_t brush_count{};
    size_t entity_count{};
};

struct VMF_File {
    size_t brush_count{};
    size_t side_count{};
    std::vector<Visgroup> visgroups{};
};

void parse_line(const std::string_view& line) {
    std::cout << line << "\n";
}

int main(const int argc, const char **argv) {
    if (argc < 2) {
        std::cout << "No files were given.\n";
        return 0;
    }

    for (int i{1}; i < argc; ++i) {
        const std::filesystem::path current_path{argv[i]};
        std::cout << "\n>>> Now opening " << current_path.filename() << "...\n";
        std::ifstream current_vmf{current_path};
        if (current_path.extension() != ".vmf") {
            std::cout << "File is not a VMF.\n";
            continue;
        }
        if (!current_vmf.is_open()) {
            std::cout << "File could not be opened.\n";
            continue;
        }
        VMF_File map{};
        for (std::string line; std::getline(current_vmf, line);) {
            boost::trim_left(line);
            parse_line(line);
        }
    }
}
