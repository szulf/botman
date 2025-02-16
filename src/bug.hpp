#pragma once

#include "constants.hpp"
#include "map.hpp"
#include "robot.hpp"

#include <string_view>
#include <vector>

// Gotta love circular dependencies
struct TexturesType;

enum class BugState : u8 {
    ALIVE,
    DEAD,
    RESPAWNING,
};

struct BugData {
public:
    BugData(const v2& pos);

    Rectangle get_collision_rect(const MapData& map_data) const;

    void render(const MapData& map_data, const TexturesType& textures) const;
    void move(float dt, const RobotData& robot_data, const MapData& map_data);
    void collide(RobotData& robot_data, MapData& map_data);

public:
    v2 pos{};
    v2 movement{};

    Texture2D texture{};
    u8 texture_frame{};
    Color tint{WHITE};

    BugState state{};
    float dead_time{};
    bool death_display{};

    // not proud of this
    float flash_delay{};

    std::vector<v2> path{};
    v2 last_pos{};
    v2 last_movement{};
    bool teleported;

};

std::string_view print_bug_state(BugState bug_state);

