#ifndef VMFOVERVIEW_UTILS_H
#define VMFOVERVIEW_UTILS_H

#include <vector>

constexpr int LARGEST_SPLIT_AMOUNT{5};
constexpr int INDEX_OF_VISGROUP_NAME_AFTER_SPLIT{3};

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
};

struct Visgroup {
    const std::string name;
    size_t brush_count{};
    size_t entity_count{};
    //TODO: add sub-visgroup support, perhaps with a visgroup variable?
    // this ties into the VMF_File usage of std::vector<Visgroup> though.
    // it is probably going to change to a different data type later -
    // would that affect the data type of this struct? Can we still have a vector?
    std::vector<Visgroup> sub_visgroups{};
};

struct VMF_File {
    size_t brush_count{};
    size_t side_count{};
    size_t entity_count{};
    //TODO: this vector may need to become a hash table in order to use the visgroup id as a key.
    std::vector<Visgroup> visgroups{};
};

void update_depth(const std::string_view &, size_t &);

Tokens line_to_token(const std::string_view &);

#endif //VMFOVERVIEW_UTILS_H
