#pragma once

#include "constants.hpp"
#include "game.hpp"
#include "map.hpp"

enum class MovementType : u8 {
    NONE,
    LEFT,
    RIGHT,
    UP,
    DOWN,
};

enum class RotationType : u8 {
    SIDE,
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
    float texture_accumulator{};
    u8 texture_frame{};
    RotationType rotation{};
    // if flipped then this is -1, if not then this is 1
    // i dont like this, but whatever
    i8 flip{};

    bool smashing_mode{};

    bool is_dead{};
    float dead_delay{};

    bool teleported{};
};

void render_robot(RobotData& robot_data, const MapData& map_data);
void robot_move(MovementType move, float dt, RobotData& robot_data, const MapData& map_data);
v2 get_grid_center(const v2& pos, const MapData& map_data);
bool in_about_center(const v2& pos, const MapData& map_data);
void robot_collect(RobotData& robot_data, MapData& map_data, GameData& game);
Rectangle robot_get_rect(const RobotData& robot_data, const MapData& map_data);
const char* print_movement(MovementType move);
