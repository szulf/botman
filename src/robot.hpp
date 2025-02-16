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

struct RobotData {
public:
    void render(const MapData& map_data, const TexturesType& textures);
    void move(Movement move, float dt, const MapData& map_data);
    void collect(MapData& map_data, GameData& game);
    Rectangle get_collision_rect(const MapData& map_data) const;

public:
    u8 lifes{3};

    v2 pos{};
    v2 movement{};

    Movement next_move{};
    float time_between_moves{};

    float texture_accumulator{};
    u8 texture_frame{};
    Flip flip{Flip::LEFT};

    bool smashing_mode{};

    bool is_dead{};
    float dead_delay{};

    bool teleported{};
};

std::string_view print_movement(Movement move);
