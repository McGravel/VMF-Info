#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include "utils.h"

void map_report(const VMF_File &vmf) {
    constexpr auto yes_or_no_text = [](const auto &item) { return item ? "Yes\n" : "No\n"; };

    std::cout << "Map Version: " << vmf.version << '\n';
    std::cout << vmf.brush_count << " brushes\n";
    std::cout << vmf.side_count << " brush sides\n";
    std::cout << vmf.entity_count << " entities\n";
    std::cout << "Has an active cordon: " << yes_or_no_text(vmf.has_active_cordon);
    std::cout << "Has in-editor camera(s): " << yes_or_no_text(vmf.has_cameras);

    if (!vmf.visgroups.empty()) {
        std::cout << vmf.visgroups.size() << " visgroups:\n";
        for (const auto &item: vmf.visgroups) {
            std::cout << "\t" << item.second.name << '\n';
        }
    }

    assert(!vmf.entities.empty());
    std::cout << "Entity list:\n";
    size_t total_entities{};
    for (const auto &item: vmf.entities) {
        total_entities += item.second;
        std::cout << '\t' << item.second << '\t' << item.first << '\n';
    }
    std::cout << total_entities << " total entities\n";
}

void parse_editor(VMF_File &vmf, std::ifstream &file, size_t &return_depth) {

}

void parse_visgroup(VMF_File &vmf, std::ifstream &file, const size_t &return_depth) {
    size_t inner_depth{return_depth};
    do {
        std::string line;
        preprocess_line(file, inner_depth, line);
        if (line_to_token(line) != Tokens::Visgroup_Single) continue;
        size_t visgroup_depth{inner_depth};

        Visgroup visgroup{};
        int id_num{-1};
        do {
            preprocess_line(file, visgroup_depth, line);
            if (line == "visgroup") {
                Visgroup inner_visgroup{};
                continue;
                //TODO: parse inner visgroup
                // Modify this function? Make a function just for sub-functions?
            }

            std::vector<std::string> split_line;
            split_line.reserve(LARGEST_SPLIT_AMOUNT);
            boost::split(split_line, line, [](const char &c) { return c == '"'; });

            for (int i = 0; i < split_line.size(); ++i) {
                const Tokens token{line_to_token(split_line[i])};
                if (token == Tokens::ID_Num_Visgroup) {
                    id_num = stoi(split_line[INDEX_OF_SPLIT_LINE_VALUE]);
                    continue;
                }
                if (token == Tokens::Name) {
                    visgroup.name = split_line[INDEX_OF_SPLIT_LINE_VALUE];
                    continue;
                }
            }
        } while (visgroup_depth > inner_depth);
        if (!visgroup.name.empty() && id_num != -1) {
            std::pair<int, Visgroup> new_group{id_num, visgroup};
            vmf.visgroups.insert(new_group);
        }
    } while (inner_depth > return_depth);
}

void parse_solid(VMF_File &vmf, std::ifstream &file, const size_t &return_depth) {
    vmf.brush_count++;
    size_t inner_depth{return_depth};
    do {
        std::string line;
        preprocess_line(file, inner_depth, line);
        if (line_to_token(line) == Tokens::Side) vmf.side_count++;
    } while (inner_depth > return_depth);
}

void update_entity_map(VMF_File &vmf, const std::vector<std::string> &split_line) {
    if (vmf.entities.contains(split_line[INDEX_OF_SPLIT_LINE_VALUE])) {
        vmf.entities[split_line[INDEX_OF_SPLIT_LINE_VALUE]] += 1;
    } else {
        vmf.entities.insert(std::make_pair(split_line[INDEX_OF_SPLIT_LINE_VALUE], 1));
    }
}

void parse_entity(VMF_File &vmf, std::ifstream &file, const size_t &return_depth) {
    //TODO: is this where we check the visgroup id and add them to the count of the appropriate one?
    vmf.entity_count++;
    size_t inner_depth{return_depth};
    do {
        std::string line;
        preprocess_line(file, inner_depth, line);

        std::vector<std::string> split_line;
        split_line.reserve(LARGEST_SPLIT_AMOUNT);
        boost::split(split_line, line, [](const char &c) { return c == '"'; });
        for (const auto &line_segment: split_line) {
            const Tokens token{line_to_token(line_segment)};
            switch (token) {
                case Tokens::Entity_Class_Name:
                    update_entity_map(vmf, split_line);
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
        }
    } while (inner_depth > return_depth);
}

void parse_group(VMF_File &vmf, std::ifstream &file, const size_t &return_depth) {
    size_t inner_depth{return_depth};
    std::string line;
    preprocess_line(file, inner_depth, line);
}

void parse_world(VMF_File &vmf, std::ifstream &file, const size_t &return_depth) {
    size_t inner_depth{return_depth};
    //Go through the 7 KeyValues of the world block
    //TODO: do something with these values?
    std::string line;
    for (int i = 0; i < 7; ++i) {
        preprocess_line(file, inner_depth, line);
    }
    //Afterward, we will begin parsing the Solid and, later on, the Group blocks inside the World block.
    do {
        preprocess_line(file, inner_depth, line);
        const Tokens token{line_to_token(line)};
        if (token == Tokens::Solid) parse_solid(vmf, file, return_depth);
        if (token == Tokens::Group) parse_group(vmf, file, return_depth);
    } while (inner_depth > return_depth);
}


void parse_cameras(VMF_File &vmf, std::ifstream &file, size_t &return_depth) {
    size_t inner_depth{return_depth};
    std::string line;
    do {
        preprocess_line(file, inner_depth, line);
        std::vector<std::string> split_line{};
        boost::split(split_line, line, [](const char &c) { return c == '"'; });
        for (const auto &line_segment: split_line) {
            if (line_segment != "activecamera") continue;
            vmf.has_cameras = split_line[INDEX_OF_SPLIT_LINE_VALUE] != "-1";
        }

    } while (inner_depth > return_depth);
}

void parse_cordon(VMF_File &vmf, std::ifstream &file, size_t &return_depth) {
    size_t inner_depth{return_depth};
    std::string line;
    do {
        preprocess_line(file, inner_depth, line);
        std::vector<std::string> split_line{};
        boost::split(split_line, line, [](const char &c) { return c == '"'; });
        for (const auto &line_segment: split_line) {
            if (line_segment != "active") continue;
            vmf.has_active_cordon = split_line[INDEX_OF_SPLIT_LINE_VALUE] == "1";
        }

    } while (inner_depth > return_depth);
}

void parse_version_info(VMF_File &vmf, std::ifstream &file, size_t &return_depth) {
    size_t inner_depth{return_depth};
    std::string line;
    preprocess_line(file, inner_depth, line);

    while (true) {
        file >> line;
        update_depth(line, inner_depth);
        if (line == "\"mapversion\"") {
            file >> line;
            vmf.version = stoi(line.substr(1, line.length() - 1));
        }

        if (inner_depth <= return_depth) return;
    }
}

void parse_hidden(VMF_File &vmf, std::ifstream &file, size_t &return_depth) {

}

void process_vmf(std::ifstream &current_vmf) {
    VMF_File map{};
    size_t depth{0};
    for (std::string line; std::getline(current_vmf, line);) {
        boost::trim_left(line);
        const Tokens token{line_to_token(line)};
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
        if (!exists(current_path)) {
            std::cout << "Invalid path given.\n";
            continue;
        }
        if (is_directory(current_path)) {
            std::cout << "Folder detected. Parsing each file in a folder is not available.\n";
            continue;
        }
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