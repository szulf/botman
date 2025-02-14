#pragma once

#include "constants.hpp"
#include "map.hpp"
#include "robot.hpp"

#include <vector>

// Gotta love circular dependencies
struct TexturesType;

enum BugStateType : u8 {
    BUG_ALIVE,
    BUG_DEAD,
    BUG_RESPAWNING,
};

struct BugData {
    v2 pos{};
    v2 movement{};

    Texture2D texture{};
    u8 texture_frame{};
    Color tint{WHITE};

    BugStateType state{};
    float dead_time{};
    bool death_display{};

    // not proud of this
    float flash_delay{};

    std::vector<v2> path{};
    v2 last_pos{};
    v2 last_movement{};
    bool teleported;
};

const char* print_bug_state(BugStateType bug_state);
BugData init_bug(const v2& pos);
Rectangle bug_get_rect(const BugData& bug_data, const MapData& map_data);
void render_bug(const BugData& bug_data, const MapData& map_data, const TexturesType& textures);
void bug_move(float dt, BugData& bug_data, const RobotData& robot_data, const MapData& map_data);
void bug_collide(BugData& bug_data, RobotData& robot_data, MapData& map_data);
