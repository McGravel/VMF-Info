#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include "utils.h"


void
map_report(const VMF_File &vmf) {
    constexpr auto yes_or_no_text = [](const auto &item) {return item ? "Yes\n" : "No\n";};

    std::cout << "Map Version: " << vmf.version << '\n';
    std::cout << vmf.brush_count << " brushes\n";
    std::cout << vmf.side_count << " brush sides\n";
    std::cout << vmf.entity_count << " entities\n";
    std::cout << "Has an active cordon: " << yes_or_no_text(vmf.has_active_cordon);
    std::cout << "Has in-editor camera(s): " << yes_or_no_text(vmf.has_cameras);

    if (!vmf.visgroups.empty()) {
        std::cout << vmf.visgroups.size() << " visgroups:\n";
        for (const auto &item : vmf.visgroups) {
            std::cout << "\t" << item.second.name << '\n';
        }
    }

    assert(!vmf.entities.empty());
    std::cout << "Entity list:\n";
    for (const auto &item : vmf.entities) {
        std::cout << '\t' << item.second << '\t' << item.first << '\n';
    }
}

void
parse_editor(VMF_File &vmf, std::ifstream &file, int &return_depth) {

}

void
parse_visgroup(VMF_File &vmf, std::ifstream &file, const int &return_depth) {
    int inner_depth { return_depth };
    std::string line;
    Visgroup visgroup {};
    int id_num { -1 };

    while (true) {
        file >> line;
        inner_depth = update_depth(line, inner_depth);

        if (line == "\"visgroupid\"") {
            file >> line;
            id_num = stoi(line.substr(1, line.length() - 1));
        }

        if (line == "\"name\"") {
            std::getline(file, line);
            visgroup.name = line;
        }

        if (inner_depth <= return_depth) break;
    }
    assert(!visgroup.name.empty() && id_num != -1 && "Incomplete visgroup!");
    std::pair<int, Visgroup> new_group { id_num, visgroup };
    vmf.visgroups.insert(new_group);
}

void
parse_solid(VMF_File &vmf, std::ifstream &file, const int &return_depth) {
    vmf.brush_count += 1;
    int inner_depth { return_depth };
    std::string line;

    while (true) {
        file >> line;
        inner_depth = update_depth(line, inner_depth);
        if (line == "side") ++vmf.side_count;

        if (inner_depth <= return_depth) return;
    }
}

void
update_entity_map(VMF_File &vmf, const std::string &line) {
    if (vmf.entities.contains(line)) {
        vmf.entities[line] += 1;
    }
    else {
        vmf.entities.insert(std::make_pair(line, 1));
    }
}

void
parse_entity(VMF_File &vmf, std::ifstream &file, const int &return_depth) {
    //TODO: is this where we check the visgroup id and add them to the count of the appropriate one?
    vmf.entity_count += 1;
    int inner_depth { return_depth };
    std::string line;

    while (true) {
        file >> line;
        inner_depth = update_depth(line, inner_depth);

        if (line == "\"classname\"") {
            file >> line;
            update_entity_map(vmf, line);
            continue;
        }
        if (line == "solid") {
            parse_solid(vmf, file, inner_depth);
            continue;
        }
        if (line == "editor") {
            parse_editor(vmf, file, inner_depth);
            continue;
        }

        if (inner_depth <= return_depth) return;
    }
}

void
parse_group(VMF_File &vmf, std::ifstream &file, const int &return_depth) {
//    int inner_depth { return_depth };
//    std::string line;
}

void
parse_world(VMF_File &vmf, std::ifstream &file, const int &return_depth) {
    int inner_depth { return_depth };
    std::string line;

    while (true) {
        file >> line;
        if (line == "solid") parse_solid(vmf, file, return_depth);
        if (line == "group") parse_group(vmf, file, return_depth);
        inner_depth = update_depth(line, inner_depth);

        if (inner_depth <= return_depth) return;
    }
}

void
parse_cameras(VMF_File &vmf, std::ifstream &file, int &return_depth) {
    int inner_depth { return_depth };
    std::string line;

    while (true) {
        file >> line;
        inner_depth = update_depth(line, inner_depth);
        if (line == "\"activecamera\"") {
            file >> line;
            vmf.has_cameras = (line != "\"-1\"");
        }

        if (inner_depth <= return_depth) return;
    }
}

void
parse_cordon(VMF_File &vmf, std::ifstream &file, int &return_depth) {
    int inner_depth { return_depth };
    std::string line;

    while (true) {
        file >> line;
        inner_depth = update_depth(line, inner_depth);
        if (line == "\"active\"") {
            file >> line;
            vmf.has_active_cordon = (line == "\"1\"");
        }

        if (inner_depth <= return_depth) return;
    }
}

void
parse_version_info(VMF_File &vmf, std::ifstream &file, int &return_depth) {
    int inner_depth { return_depth };
    std::string line;

    while (true) {
        file >> line;
        inner_depth = update_depth(line, inner_depth);
        if (line == "\"mapversion\"") {
            file >> line;
            vmf.version = stoi(line.substr(1, line.length() - 1));
        }

        if (inner_depth <= return_depth) return;
    }
}

void
parse_hidden(VMF_File &vmf, std::ifstream &file, int &return_depth) {

}

void
process_vmf(std::ifstream &current_vmf) {
    VMF_File map {};
    int depth { 0 };
    for (std::string line; current_vmf >> line;) {
        update_depth(line, depth);
        const Tokens token { line_to_token(line) };
        switch (token) {
            case Tokens::Version_Info:
                parse_version_info(map, current_vmf, depth);
                break;
            case Tokens::Visgroup_Single:
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

int
main(const int argc, const char **argv) {
    if (argc < 2) {
        std::cout << "No files were given.\n";
#if defined(__linux__) || defined(__MINGW64__)
        std::cout << "Usage: VMFOverview path/to/map1.vmf path/to/map2.vmx ...\n";
#elif defined(_WIN32)
        std::cout << "Usage: VMFOverview.exe \"path\\to\\map1.vmf\" \"path\\to\\map2.vmx\" ...\n";
#endif
        return 0;
    }

    for (int i { 1 }; i < argc; ++i) {
        const std::filesystem::path current_path { argv[i] };
        std::cout << "\n>>> Now opening " << current_path.filename() << "...\n";
        std::ifstream current_vmf { current_path };
        if (!exists(current_path)) {
            std::cout << "Invalid path given.\n";
            continue;
        }
        if (is_directory(current_path)) {
            std::cout << "Folder detected. Parsing each file in a folder is not available.\n";
            continue;
        }
        if (current_path.extension() != ".vmf" && current_path.extension() != ".vmx") {
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
