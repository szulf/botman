#pragma once

#include "constants.hpp"
#include "map.hpp"

// Gotta love circular dependencies
struct GameData;
struct TexturesType;

enum class Movement : u8 {
    NONE,
    LEFT,
    RIGHT,
    UP,
    DOWN,
};

enum class Flip : i8 {
    LEFT = -1,
    RIGHT = 1,
};

enum class RobotState : u8 {
    NORMAL,
    SMASHING,
    DYING,
};

struct RobotData {
public:
    RobotData() = default;
    RobotData(const v2& pos, u16 lifes) : lifes{lifes}, pos{pos} { }

    Rectangle collision_rect(const MapData& map_data) const;

    void render(const MapData& map_data, const TexturesType& textures) const;
    void move(Movement move, float dt, const MapData& map_data);
    bool collect(MapData& map_data, GameData& game);

public:
    u16 lifes{};

    v2 pos{};
    v2 movement{-1, 0};

    Movement next_move{Movement::LEFT};
    float time_between_moves{};

    Flip flip{Flip::LEFT};

    RobotState state{};

    float dead_delay{};

    bool teleported{};
};

std::string_view print_movement(Movement move);
