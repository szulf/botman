#include "bug.hpp"
#include "path.hpp"

#include "raymath.h"

const char* print_bug_state(BugStateType bug_state) {
    switch (bug_state) {
        case BugStateType::ALIVE:
            return "alive";
        case BugStateType::DEAD:
            return "dead";
        case BugStateType::RESPAWNING:
            return "respawning";
    }

    return "";
}

BugData init_bug(const v2& pos, u8 idx) {
    return BugData{
        .pos = pos,
        .texture = LoadTexture(ROOT_PATH "/assets/bug.png"),
        .state = BugStateType::RESPAWNING,
        .dead_time = static_cast<float>(GetTime() + idx),
    };
}

Rectangle bug_get_rect(const BugData& bug_data, const MapData& map_data) {
    return {bug_data.pos.x - map_data.GRID_WIDTH / 2.0f, bug_data.pos.y - map_data.GRID_HEIGHT / 2.0f, static_cast<float>(map_data.GRID_WIDTH), static_cast<float>(map_data.GRID_HEIGHT)};
}

void render_bug(const BugData& bug_data, const MapData& map_data) {
    if (bug_data.death_display) {
        DrawText("100", bug_data.pos.x - 20, bug_data.pos.y - 25, 20, GREEN);
    }

    DrawTexturePro(bug_data.texture, {static_cast<float>(map_data.GRID_WIDTH * bug_data.texture_frame), 0, static_cast<float>(map_data.GRID_WIDTH), static_cast<float>(map_data.GRID_HEIGHT)}, {bug_data.pos.x, bug_data.pos.y, static_cast<float>(map_data.GRID_WIDTH), static_cast<float>(map_data.GRID_HEIGHT)}, {map_data.GRID_WIDTH / 2.0f, map_data.GRID_HEIGHT / 2.0f}, 0.0f, bug_data.tint);
}

void bug_move(float dt, BugData& bug_data, const RobotData& robot_data, const MapData& map_data) {
    if (bug_data.state == BugStateType::DEAD && GetTime() - bug_data.dead_time < 1) {
        bug_data.death_display = true;
        if (GetTime() - bug_data.flash_delay > 0.2) {
            bug_data.tint.a = bug_data.tint.a == 255 ? 80 : 255;
            bug_data.flash_delay = GetTime();
        }
        return;
    }

    if (bug_data.state == BugStateType::DEAD) {
        bug_data.death_display = false;
        bug_data.tint.a = 80;
    }

    if (bug_data.state == BugStateType::DEAD && in_about_center(bug_data.pos, map_data) && get_grid_from_pos(bug_data.pos, map_data) == map_data.spawner_pos) {
        bug_data.state = BugStateType::RESPAWNING;
        bug_data.dead_time = GetTime();
        bug_data.pos = get_grid_center(bug_data.pos, map_data);
        bug_data.tint.a = 255;
        return;
    }

    if (bug_data.state == BugStateType::RESPAWNING && GetTime() - bug_data.dead_time > 1) {
        bug_data.state = BugStateType::ALIVE;
        return;
    }

    if (bug_data.state == BugStateType::RESPAWNING) {
        return;
    }


    auto grid_pos = get_grid_from_pos(bug_data.pos, map_data);

    if (in_about_center(bug_data.pos, map_data) && bug_data.last_pos != grid_pos) {
        if (bug_data.state == BugStateType::DEAD) {
            bug_data.path = find_path(grid_pos, map_data.spawner_pos, map_data);
        } else if (robot_data.smashing_mode) {
            bug_data.path = find_path(grid_pos, find_furthest_grid_pos(get_grid_from_pos(robot_data.pos, map_data), map_data), map_data);
        } else {
            bug_data.path = find_path(grid_pos, get_grid_from_pos(robot_data.pos, map_data), map_data);
        }
        if (bug_data.path.size() == 0) {
            bug_data.pos = get_grid_center(bug_data.pos, map_data);
            return;
        }
        bug_data.movement = bug_data.path[0] - grid_pos;
        bug_data.last_pos = grid_pos;
        if (bug_data.movement != bug_data.last_movement) {
            bug_data.pos = get_grid_center(bug_data.pos, map_data);
            bug_data.last_movement = bug_data.movement;
        }
    }

    v2 next_grid_pos = get_grid_from_pos(bug_data.pos, map_data) + bug_data.movement;
    v2 next_pos = get_pos_from_grid(next_grid_pos, map_data);
    if (
            (get_tile(next_grid_pos, map_data) == TileType::WALL || (get_tile(next_grid_pos, map_data) == TileType::SPAWNER && next_grid_pos != bug_data.path.back())) &&
            CheckCollisionRecs(
                {
                    next_pos.x - map_data.GRID_WIDTH / 2.0f,
                    next_pos.y - map_data.GRID_HEIGHT / 2.0f,
                    static_cast<float>(map_data.GRID_WIDTH),
                    static_cast<float>(map_data.GRID_HEIGHT)
                },
                bug_get_rect(bug_data, map_data)
            )
        ) {
        bug_data.pos = get_grid_center(bug_data.pos, map_data);
        return;
    } else {
        if (get_tile(grid_pos, map_data) == TileType::PORTAL) {
            if (!bug_data.teleported && in_about_center(bug_data.pos, map_data)) {
                bug_data.pos = get_pos_from_grid(get_second_portal_pos(grid_pos, map_data), map_data);
                bug_data.teleported = true;
            }
        } else {
            bug_data.teleported = false;
        }

        bug_data.pos -= bug_data.movement * dt * MOVEMENT_SPEED * 0.85f;
    }
}

void bug_collide(BugData& bug_data, RobotData& robot_data, MapData& map_data) {
    if (bug_data.state != BugStateType::ALIVE) {
        return;
    }

    if (CheckCollisionRecs(bug_get_rect(bug_data, map_data), robot_get_rect(robot_data, map_data))) {
        if (robot_data.smashing_mode) {
            bug_data.state = BugStateType::DEAD;
            bug_data.dead_time = GetTime();
            map_data.score += 100;
        } else {
            robot_data.lifes -= 1;
            robot_data.is_dead = true;
            robot_data.dead_delay = GetTime();
        }
    }
}
