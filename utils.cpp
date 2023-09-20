#include <string>
#include <cassert>
#include "utils.h"


Tokens line_to_token(const std::string_view &line) {
    if (line == "{") return Tokens::Brace_Open;
    if (line == "}") return Tokens::Brace_Close;
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

void update_depth(const std::string_view &line, size_t &depth) {
    assert(depth >= 0 && "Depth check failed.");
    if (line == "{") depth++;
    if (line == "}") depth--;
}
