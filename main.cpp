#include <boost/algorithm/string.hpp>
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
    const std::string name;
    size_t brush_count{};
    size_t entity_count{};
};

struct VMF_File {
    size_t brush_count{};
    size_t side_count{};
    std::vector<Visgroup> visgroups{};
};

inline void update_depth(const std::string_view &line, size_t &depth) {
    assert(depth >= 0 && "Depth check failed.");
    if (line == "{") depth++;
    if (line == "}") depth--;
}

Tokens evaluate_token(const std::string_view &line) {
    if (line == "{") return Tokens::Brace_Open;
    if (line == "}") return Tokens::Brace_Close;
    if (line == "visgroups") return Tokens::Visgroup_Block;
    if (line == "visgroup") return Tokens::Visgroup_Single;
    if (line == "solid") return Tokens::Solid;
    if (line == "entity") return Tokens::Entity;
    if (line == "side") return Tokens::Side;
    return Tokens::None;
}

void parse_visgroup(VMF_File &vmf, std::ifstream &file, const size_t &return_depth) {
    size_t inner_depth{return_depth};
    do {
        std::string line;
        std::getline(file, line);
        boost::trim_left(line);
        update_depth(line, inner_depth);
        //TODO: figure out how to determine sub-visgroups (a visgroup inside a visgroup).
        // Considering using depth, or perhaps amount of tabs in the line.
        if (evaluate_token(line) == Tokens::Visgroup_Single) {
            size_t visgroup_depth{inner_depth};
            do {
                std::getline(file, line);
                boost::trim_left(line);
                update_depth(line, visgroup_depth);

                std::vector<std::string> split_line;
                split_line.reserve(3);
                boost::split(split_line, line, [](const char &c) { return c == '"'; });
                for (int i = 0; i < split_line.size(); ++i) {
                    if (split_line[i] == "name") {
                        Visgroup visgroup{.name = split_line[3]};
                        vmf.visgroups.push_back(visgroup);
                        break;
                    }
                }
            } while (visgroup_depth > inner_depth);
        }
    } while (inner_depth > return_depth);
}

void parse_file(std::ifstream &current_vmf) {
    VMF_File map{};
    size_t depth{0};
    for (std::string line; std::getline(current_vmf, line);) {
        boost::trim_left(line);
        Tokens token{evaluate_token(line)};
        switch (token) {
            case Tokens::Brace_Open:
                depth++;
                break;
            case Tokens::Brace_Close:
                depth--;
                break;
            case Tokens::Visgroup_Block:
                parse_visgroup(map, current_vmf, depth);
                break;
            case Tokens::Visgroup_Single:
                assert(false && "Individual visgroup detected outside of Visgroups block?");
            case Tokens::Solid:
                break;
            case Tokens::Entity:
                break;
            case Tokens::Side:
                break;
            default:
                break;
        }
    }
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
        parse_file(current_vmf);

    }
}
