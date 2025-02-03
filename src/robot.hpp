#pragma once

#include "constants.hpp"
#include "map.hpp"

enum class MovementType : u8 {
    NONE,
    LEFT,
    RIGHT,
    UP,
    DOWN,
};

struct RobotData {
    u8 lifes{3};

    v2 pos{};
    v2 movement{};

    MovementType next_move{};
    float time_between_moves{};

    Texture2D texture{};
    u8 texture_frame{};

    bool smashing_mode{};

    bool is_dead{};
    float dead_delay{};
};

void render_robot(const RobotData& robot_data, const MapData& map_data);
void robot_move(MovementType move, float dt, RobotData& robot_data, const MapData& map_data);
v2 get_grid_center(const v2& pos, const MapData& map_data);
bool in_about_center(const v2& pos, const MapData& map_data);
void robot_collect(RobotData& robot_data, MapData& map_data);
Rectangle robot_get_rect(const RobotData& robot_data, const MapData& map_data);
const char* print_movement(MovementType move);
