#pragma once

#include "constants.hpp"
#include "map.hpp"

#include "raymath.h"
#include <vector>

struct Node {
    v2 pos;
    v2 parent;

    float f{};
    float g{};
    float h{};

    bool operator==(const Node& other) const {
        return pos == other.pos;
    }
};

enum QuadrantType : u8 {
    QUAD_TOP_LEFT,
    QUAD_TOP_RIGHT,
    QUAD_BOTTOM_LEFT,
    QUAD_BOTTOM_RIGHT,
};

v2 find_furthest_grid_pos(const v2& grid_pos, const MapData& map_data);
std::vector<v2> find_path(const v2& start_grid_pos, const v2& end_grid_pos, const MapData& map_data);
