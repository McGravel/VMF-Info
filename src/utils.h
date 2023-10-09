#ifndef VMFOVERVIEW_UTILS_H
#define VMFOVERVIEW_UTILS_H

#include <vector>
#include <map>
#include <unordered_map>

inline constexpr int LARGEST_SPLIT_AMOUNT{5};
inline constexpr int INDEX_OF_SPLIT_LINE_VALUE{3};

enum class Tokens {
    None,
    Brace_Open,
    Brace_Close,
    Visgroup_Block,
    Visgroup_Single,
    Solid,
    Entity,
    Side,
    World,
    Group,
    Version_Info,
    Hidden,
    Cameras,
    Cordon,
    Editor_Block,
    Entity_Class_Name,
    ID_Num_Visgroup,
    ID_Num,
    Name,
};

struct Visgroup {
    std::string name;
    int brush_count{};
    int entity_count{};
    //TODO: add sub-visgroup support, perhaps with a visgroup variable?
    // this ties into the VMF_File usage of std::vector<Visgroup> though.
    // it is probably going to change to a different data type later -
    // would that affect the data type of this struct? Can we still have a vector?
    std::unordered_map<int, Visgroup> inner_visgroups{};
};

struct VMF_File {
    bool has_cameras{false};
    bool has_active_cordon{false};
    int version{};
    int brush_count{};
    int side_count{};
    int entity_count{};
    std::unordered_map<int, Visgroup> visgroups{};
    std::map<std::string, int> entities{};
};

void update_depth(const std::string_view &line, int &depth);

Tokens line_to_token(const std::string_view &line);

void preprocess_line(std::ifstream &file, int &depth, std::string &line);

#endif //VMFOVERVIEW_UTILS_H
