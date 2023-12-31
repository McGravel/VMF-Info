#include <string>
#include <cassert>
#include "utils.h"


Tokens
line_to_token(const std::string_view line) {
    if (line == "{") return Tokens::Brace_Open;
    if (line == "}") return Tokens::Brace_Close;
    if (line == "id") return Tokens::ID_Num;
    if (line == "visgroupid") return Tokens::ID_Num_Visgroup;
    if (line == "name") return Tokens::Name;
    if (line == "versioninfo") return Tokens::Version_Info;
    if (line == "visgroups") return Tokens::Visgroup_Block;
    if (line == "visgroup") return Tokens::Visgroup_Single;
    if (line == "world") return Tokens::World;
    if (line == "solid") return Tokens::Solid;
    if (line == "entity") return Tokens::Entity;
    if (line == "side") return Tokens::Side;
    if (line == "group") return Tokens::Group;
    if (line == "hidden") return Tokens::Hidden;
    if (line == "cameras") return Tokens::Cameras;
    if (line == "cordon") return Tokens::Cordon;
    if (line == "editor") return Tokens::Editor_Block;
    if (line == "classname") return Tokens::Entity_Class_Name;

    return Tokens::None;
}

int
update_depth(const std::string_view line, int depth) {
    switch (line[0]) {
        case '{':
            return depth + 1;
        case '}':
            return depth - 1;
        default:
            assert(false && "Update Depth called on invalid character/string");
            return depth;
    }
}
