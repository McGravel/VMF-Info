#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include "utils.h"


void parse_editor(VMF_File &vmf, std::ifstream &file, size_t &return_depth) {

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
        // EDIT: after checking how the file is laid out, depth should work for us.
        if (line_to_token(line) != Tokens::Visgroup_Single) continue;
        size_t visgroup_depth{inner_depth};
        do {
            std::getline(file, line);
            boost::trim_left(line);
            update_depth(line, visgroup_depth);

            std::vector<std::string> split_line;
            constexpr int LARGEST_SPLIT_AMOUNT{5};
            split_line.reserve(LARGEST_SPLIT_AMOUNT);
            boost::split(split_line, line, [](const char &c) { return c == '"'; });
            for (int i = 0; i < split_line.size(); ++i) {
                if (split_line[i] == "name") {
                    //Based on current splitting of the string, the line containing the name of the visgroup
                    // is the 4th element of the vector that boost::split produces.
                    constexpr int INDEX_OF_VISGROUP_NAME_AFTER_SPLIT{3};
                    Visgroup visgroup{.name = split_line[INDEX_OF_VISGROUP_NAME_AFTER_SPLIT]};
                    //TODO: further visgroup parsing, will have to be done outside of this function?
                    // probably requires a hash map (unordered_map) in order to convert the visgroup ID into
                    // a key for later access?
                    vmf.visgroups.push_back(visgroup);
                    break;
                }
            }
        } while (visgroup_depth > inner_depth);
    } while (inner_depth > return_depth);
}

void parse_solid(VMF_File &vmf, std::ifstream &file, const size_t &return_depth) {
    vmf.brush_count++;
    size_t inner_depth{return_depth};
    do {
        std::string line;
        std::getline(file, line);
        boost::trim_left(line);
        update_depth(line, inner_depth);
        if (line_to_token(line) == Tokens::Side) vmf.side_count++;
    } while (inner_depth > return_depth);
}

void parse_entity(VMF_File &vmf, std::ifstream &file, const size_t &return_depth) {
    //TODO: is this where we check the visgroup id and add them to the count of the appropriate one?
    vmf.entity_count++;
    size_t inner_depth{return_depth};
    do {
        std::string line;
        std::getline(file, line);
        boost::trim_left(line);
        update_depth(line, inner_depth);
        Tokens token{line_to_token(line)};
        switch (token) {
            case Tokens::Entity_Class_Name:
                //TODO: add class name to a map somewhere where we can count amount!
                // classname should be the key, to a value of an int we can modify.
                printf("Class Name: %s", line.c_str());
                break;
            case Tokens::Solid:
                parse_solid(vmf, file, inner_depth);
                break;
            case Tokens::Editor_Block:
                parse_editor(vmf, file, inner_depth);
                break;
            default:
                break;
        }
    } while (inner_depth > return_depth);
}

void parse_group(VMF_File &vmf, std::ifstream &file, const size_t &depth) {

}

void parse_world(VMF_File &vmf, std::ifstream &file, const size_t &return_depth) {
    size_t inner_depth{return_depth};
    //Go through the 7 KeyValues of the world block
    //TODO: do something with these values?
    std::string line;
    for (int i = 0; i < 7; ++i) {
        std::getline(file, line);
        boost::trim_left(line);
        update_depth(line, inner_depth);
    }
    //Afterward, we will begin parsing the Solid and, later on, the Group blocks inside the World block.
    do {
        std::getline(file, line);
        boost::trim_left(line);
        update_depth(line, inner_depth);
        Tokens token{line_to_token(line)};
        if (token == Tokens::Solid) parse_solid(vmf, file, return_depth);
        if (token == Tokens::Group) parse_group(vmf, file, return_depth);
    } while (inner_depth > return_depth);
}

void map_report(const VMF_File &vmf) {
    std::cout << '\t' << vmf.brush_count << " brushes\n";
    std::cout << '\t' << vmf.side_count << " brush sides\n";
    std::cout << '\t' << vmf.entity_count << " entities\n";
    std::cout << '\t' << vmf.visgroups.size() << " visgroups:\n";

    if (vmf.visgroups.empty()) return;
    for (const auto &item: vmf.visgroups) {
        std::cout << "\t\t" << item.name << '\n';
    }
}

void parse_cameras(VMF_File &vmf, std::ifstream &file, size_t &return_depth) {

}

void parse_cordon(VMF_File &vmf, std::ifstream &file, size_t &return_depth) {

}

void parse_version_info(VMF_File &vmf, std::ifstream &file, size_t &return_depth) {

}

void parse_hidden(VMF_File &vmf, std::ifstream &file, size_t &return_depth) {

}

void process_vmf(std::ifstream &current_vmf) {
    VMF_File map{};
    size_t depth{0};
    for (std::string line; std::getline(current_vmf, line);) {
        boost::trim_left(line);
        Tokens token{line_to_token(line)};
        switch (token) {
            case Tokens::Brace_Open:
                depth++;
                break;
            case Tokens::Brace_Close:
                depth--;
                break;
            case Tokens::Version_Info:
                parse_version_info(map, current_vmf, depth);
                break;
            case Tokens::Visgroup_Block:
                parse_visgroup(map, current_vmf, depth);
                break;
            case Tokens::World:
                parse_world(map, current_vmf, depth);
                break;
            case Tokens::Entity:
                parse_entity(map, current_vmf, depth);
                break;
            case Tokens::Cameras:
                parse_cameras(map, current_vmf, depth);
                break;
            case Tokens::Cordon:
                parse_cordon(map, current_vmf, depth);
                break;
            case Tokens::Hidden:
                parse_hidden(map, current_vmf, depth);
                break;
            default:
                break;
        }
    }
    map_report(map);
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
        process_vmf(current_vmf);

    }
}
