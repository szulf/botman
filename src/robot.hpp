#pragma once

#include "constants.hpp"
#include "map.hpp"

// Gotta love circular dependencies
struct GameData;
struct TexturesType;

enum MovementType : u8 {
    MOVE_NONE,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_UP,
    MOVE_DOWN,
};

enum FlipType : i8 {
    FLIP_LEFT = -1,
    FLIP_RIGHT = 1,
};

struct RobotData {
    u8 lifes{3};

    v2 pos{};
    v2 movement{};

    MovementType next_move{};
    float time_between_moves{};

    float texture_accumulator{};
    u8 texture_frame{};
    FlipType flip{FLIP_LEFT};

    bool smashing_mode{};

    bool is_dead{};
    float dead_delay{};

    bool teleported{};
};

void render_robot(RobotData& robot_data, const MapData& map_data, const TexturesType& textures);
void robot_move(MovementType move, float dt, RobotData& robot_data, const MapData& map_data);
v2 get_grid_center(const v2& pos, const MapData& map_data);
bool in_about_center(const v2& pos, const MapData& map_data);
void robot_collect(RobotData& robot_data, MapData& map_data, GameData& game);
Rectangle robot_get_rect(const RobotData& robot_data, const MapData& map_data);
const char* print_movement(MovementType move);
