#include "bug.hpp"
#include "path.hpp"
#include "game.hpp"

#include "raylib.h"
#include "raymath.h"
#include <cstdio>

std::string_view print_bug_state(BugState bug_state) {
    switch (bug_state) {
        case BugState::ALIVE:
            return "alive";
        case BugState::DEAD:
            return "dead";
        case BugState::RESPAWNING:
            return "respawning";
    }

    return "";
}

void set_bugs_dead_time(std::vector<BugData>& bugs) {
    float time = GetTime();
    for (u8 i = 1; auto& bug : bugs) {
        bug.dead_time = time + i;
        i++;
    }
}

Rectangle BugData::collision_rect(const MapData& map_data) const {
    return {pos.x - map_data.GRID_WIDTH / 2.0f, pos.y - map_data.GRID_HEIGHT / 2.0f, static_cast<float>(map_data.GRID_WIDTH), static_cast<float>(map_data.GRID_HEIGHT)};
}

void BugData::render(const MapData& map_data, const TexturesType& textures, u8 idx) const {
    if (death_display) {
        DrawText("100", pos.x - 20, pos.y - 25, 20, GREEN);
    }

    DrawTexturePro(textures.bug.texture, {static_cast<float>(textures.bug.width * textures.bug.frame[idx]), 0, static_cast<float>(textures.bug.width), static_cast<float>(textures.bug.height)}, {pos.x, pos.y, static_cast<float>(map_data.GRID_WIDTH), static_cast<float>(map_data.GRID_HEIGHT)}, {map_data.GRID_WIDTH / 2.0f, map_data.GRID_HEIGHT / 2.0f}, 0.0f, tint);
}

void BugData::move(float dt, const RobotData& robot_data, const MapData& map_data) {
    if (state == BugState::DEAD && GetTime() - dead_time < 1) {
        death_display = true;
        if (GetTime() - flash_delay > 0.2) {
            tint.a = tint.a == 255 ? 80 : 255;
            flash_delay = GetTime();
        }
        moving = false;
        return;
    }

    if (state == BugState::DEAD) {
        death_display = false;
        tint.a = 80;
        moving = false;
    }

    if (state == BugState::DEAD && map_data.in_about_center(pos) && map_data.get_grid_from_pos(pos) == map_data.spawner_pos) {
        state = BugState::RESPAWNING;
        dead_time = GetTime();
        pos = map_data.get_grid_center(pos);
        tint.a = 0;
        moving = false;
        return;
    }

    if (state == BugState::RESPAWNING && GetTime() - dead_time > 1) {
        state = BugState::ALIVE;
        tint.a = 255;
        moving = false;
        return;
    }

    if (state == BugState::RESPAWNING) {
        moving = false;
        return;
    }

    auto grid_pos = map_data.get_grid_from_pos(pos);

    if (map_data.in_about_center(pos) && last_pos != grid_pos) {
        if (state == BugState::DEAD) {
            path = find_path(grid_pos, map_data.spawner_pos, map_data);
        } else if (robot_data.smashing_mode) {
            path = find_path(grid_pos, find_furthest_grid_pos(map_data.get_grid_from_pos(robot_data.pos), map_data), map_data);
        } else {
            path = find_path(grid_pos, map_data.get_grid_from_pos(robot_data.pos), map_data);
        }
        if (path.size() == 0) {
            pos = map_data.get_grid_center(pos);
            return;
        }
        movement = path[0] - grid_pos;
        last_pos = grid_pos;
        if (movement != last_movement) {
            pos = map_data.get_grid_center(pos);
            last_movement = movement;
        }
    }

    v2 next_grid_pos = map_data.get_grid_from_pos(pos) + movement;
    v2 next_pos = map_data.get_pos_from_grid(next_grid_pos);
    if (
            (map_data.get_tile(next_grid_pos) == Tile::WALL || (map_data.get_tile(next_grid_pos) == Tile::SPAWNER && next_grid_pos != path.back())) &&
            CheckCollisionRecs(
                {
                    next_pos.x - map_data.GRID_WIDTH / 2.0f,
                    next_pos.y - map_data.GRID_HEIGHT / 2.0f,
                    static_cast<float>(map_data.GRID_WIDTH),
                    static_cast<float>(map_data.GRID_HEIGHT)
                },
                collision_rect(map_data)
            )
        ) {
        pos = map_data.get_grid_center(pos);
        moving = false;
        return;
    } else {
        if (map_data.get_tile(grid_pos) == Tile::PORTAL) {
            if (!teleported && map_data.in_about_center(pos)) {
                pos = map_data.get_pos_from_grid(map_data.get_second_portal_pos(grid_pos));
                teleported = true;
                return;
            }
        } else {
            teleported = false;
        }

        pos -= movement * dt * MOVEMENT_SPEED * 0.85f;
        moving = true;
    }
}

void BugData::collide(RobotData& robot_data, MapData& map_data) {
    if (state != BugState::ALIVE) {
        return;
    }

    if (CheckCollisionRecs(collision_rect(map_data), robot_data.collision_rect(map_data))) {
        if (robot_data.smashing_mode) {
            state = BugState::DEAD;
            dead_time = GetTime();
            map_data.score += 100;
        } else {
            if (!robot_data.is_dead) {
                robot_data.lifes -= 1;
            }
            robot_data.is_dead = true;
            robot_data.dead_delay = GetTime();
        }
    }
}

