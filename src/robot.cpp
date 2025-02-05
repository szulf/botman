#include "robot.hpp"
#include "map.hpp"
#include "raylib.h"
#include "raymath.h"
#include <cstdio>

void render_robot(RobotData& robot_data, const MapData& map_data) {
    robot_data.texture_frame = fmod(robot_data.texture_accumulator * 10.0f, 8.0f);

    DrawTexturePro(robot_data.texture, {static_cast<float>(map_data.GRID_WIDTH * robot_data.texture_frame), static_cast<float>(map_data.GRID_HEIGHT * static_cast<u8>(robot_data.rotation)), static_cast<float>(map_data.GRID_WIDTH / 2.0f * robot_data.flip), static_cast<float>(map_data.GRID_HEIGHT / 2.0f)}, {robot_data.pos.x, robot_data.pos.y, static_cast<float>(map_data.GRID_WIDTH), static_cast<float>(map_data.GRID_HEIGHT)}, {map_data.GRID_WIDTH / 2.0f, map_data.GRID_HEIGHT / 2.0f}, 0.0f, WHITE);
}

void robot_move(MovementType move, float dt, RobotData& robot_data, const MapData& map_data) {
    switch (move) {
        case MovementType::UP:
            if (robot_data.movement == v2{0, 1})
            {
                robot_data.movement = {0, -1};

                robot_data.rotation = RotationType::UP;
                robot_data.flip = 1;
            }
            else
            {
                robot_data.next_move = move;
                robot_data.time_between_moves = GetTime();
            }
            break;

        case MovementType::DOWN:
            if (robot_data.movement == v2{0, -1})
            {
                robot_data.movement = {0, 1};

                robot_data.rotation = RotationType::DOWN;
                robot_data.flip = 1;
            }
            else
            {
                robot_data.next_move = move;
                robot_data.time_between_moves = ::GetTime();
            }
            break;

        case MovementType::LEFT:
            if (robot_data.movement == v2{1, 0})
            {
                robot_data.movement = {-1, 0};

                robot_data.rotation = RotationType::SIDE;
                robot_data.flip = -1;
            }
            else
            {
                robot_data.next_move = move;
                robot_data.time_between_moves = ::GetTime();
            }
            break;

        case MovementType::RIGHT:
            if (robot_data.movement == v2{-1, 0})
            {
                robot_data.movement = {1, 0};

                robot_data.rotation = RotationType::SIDE;
                robot_data.flip = 1;
            }
            else
            {
                robot_data.next_move = move;
                robot_data.time_between_moves = ::GetTime();
            }
            break;

        case MovementType::NONE:
            break;
    }

    auto grid_pos = get_grid_from_pos(robot_data.pos, map_data);
    if (in_about_center(robot_data.pos, map_data) && robot_data.next_move != MovementType::NONE) {
        static bool first = true;
        if (GetTime() - robot_data.time_between_moves >= 0.35f && !first)
        {
            robot_data.next_move = MovementType::NONE;
        }
        first = false;

        switch (robot_data.next_move) {
            case MovementType::LEFT: {
                if (robot_data.movement == v2{-1, 0}) {
                    break;
                }
                v2 next_movement = {-1, 0};
                auto next_pos = grid_pos + next_movement;
                auto next_tile = get_tile(next_pos, map_data);
                if (next_tile != TileType::WALL && next_tile != TileType::SPAWNER) {
                    robot_data.movement = next_movement;
                    robot_data.pos = get_grid_center(robot_data.pos, map_data);
                    robot_data.next_move = MovementType::NONE;

                    robot_data.rotation = RotationType::SIDE;
                    robot_data.flip = -1;
                }
                break;
            }

            case MovementType::RIGHT: {
                if (robot_data.movement == v2{1, 0}) {
                    break;
                }
                v2 next_movement = {1, 0};
                auto next_pos = grid_pos + next_movement;
                auto next_tile = get_tile(next_pos, map_data);
                if (next_tile != TileType::WALL && next_tile != TileType::SPAWNER) {
                    robot_data.movement = next_movement;
                    robot_data.pos = get_grid_center(robot_data.pos, map_data);
                    robot_data.next_move = MovementType::NONE;

                    robot_data.rotation = RotationType::SIDE;
                    robot_data.flip = 1;
                }
                break;
            }

            case MovementType::UP: {
                if (robot_data.movement == v2{0, -1}) {
                    break;
                }
                v2 next_movement = {0, -1};
                auto next_pos = grid_pos + next_movement;
                auto next_tile = get_tile(next_pos, map_data);
                if (next_tile != TileType::WALL && next_tile != TileType::SPAWNER) {
                    robot_data.movement = next_movement;
                    robot_data.pos = get_grid_center(robot_data.pos, map_data);
                    robot_data.next_move = MovementType::NONE;

                    robot_data.rotation = RotationType::UP;
                    robot_data.flip = 1;
                }
                break;
            }

            case MovementType::DOWN: {
                if (robot_data.movement == v2{0, 1}) {
                    break;
                }
                v2 next_movement = {0, 1};
                auto next_pos = grid_pos + next_movement;
                auto next_tile = get_tile(next_pos, map_data);
                if (next_tile != TileType::WALL && next_tile != TileType::SPAWNER) {
                    robot_data.movement = next_movement;
                    robot_data.pos = get_grid_center(robot_data.pos, map_data);
                    robot_data.next_move = MovementType::NONE;

                    robot_data.rotation = RotationType::DOWN;
                    robot_data.flip = 1;
                }
                break;
            }

            case MovementType::NONE:
                break;
        }
    }

    auto next_grid_pos = grid_pos + robot_data.movement;
    auto next_pos = get_pos_from_grid(next_grid_pos, map_data);
    if (
            (get_tile(next_grid_pos, map_data) == TileType::WALL || get_tile(next_grid_pos, map_data) == TileType::SPAWNER) &&
            CheckCollisionRecs(
                {next_pos.x - map_data.GRID_WIDTH / 2.0f, next_pos.y - map_data.GRID_HEIGHT / 2.0f, static_cast<float>(map_data.GRID_WIDTH), static_cast<float>(map_data.GRID_HEIGHT)},
                robot_get_rect(robot_data, map_data)
            )
        ) {
        robot_data.movement = {0, 0};
        robot_data.pos = get_grid_center(robot_data.pos, map_data);
        robot_data.texture_accumulator = 0.0f;
    } else {
        if (get_tile(get_grid_from_pos(robot_data.pos, map_data), map_data) == TileType::PORTAL) {
            if (!robot_data.teleported && in_about_center(robot_data.pos, map_data)) {
                robot_data.pos = get_pos_from_grid(get_second_portal_pos(grid_pos, map_data), map_data);
                robot_data.teleported = true;
            }
        } else {
            robot_data.teleported = false;
        }

        robot_data.pos -= robot_data.movement * dt * MOVEMENT_SPEED;
        robot_data.texture_accumulator += dt;
    }
}

v2 get_grid_center(const v2& pos, const MapData& map_data) {
    return get_pos_from_grid(get_grid_from_pos(pos, map_data), map_data);
}

bool in_about_center(const v2& pos, const MapData& map_data) {
    auto center_pos = get_grid_center(pos, map_data);
    return (center_pos.x - 4 <= pos.x && center_pos.x + 4 >= pos.x) && (center_pos.y - 4 <= pos.y && center_pos.y + 4 >= pos.y);
}

void robot_collect(RobotData& robot_data, MapData& map_data) {
    auto grid_pos = get_grid_from_pos(robot_data.pos, map_data);

    if (get_tile(grid_pos, map_data) == TileType::PELLET && CheckCollisionRecs({robot_data.pos.x - (map_data.GRID_WIDTH / 8.0f), robot_data.pos.y - (map_data.GRID_HEIGHT / 8.0f), map_data.GRID_WIDTH / 4.0f, map_data.GRID_HEIGHT / 4.0f}, robot_get_rect(robot_data, map_data))) {
        set_tile(grid_pos, TileType::EMPTY, map_data);
        map_data.score += 10;
        map_data.pellet_count--;
        if (map_data.pellet_count == 0) {
            map_data.state = GameStateType::WON;
        }
    }

    static float smashing_start{};
    if (get_tile(grid_pos, map_data) == TileType::HAMMER && CheckCollisionRecs({robot_data.pos.x - (map_data.GRID_WIDTH / 4.0f), robot_data.pos.y - (map_data.GRID_HEIGHT / 4.0f), map_data.GRID_WIDTH / 2.0f, map_data.GRID_HEIGHT / 2.0f}, robot_get_rect(robot_data, map_data))) {
        set_tile(grid_pos, TileType::EMPTY, map_data);
        robot_data.smashing_mode= true;
        smashing_start = GetTime();
    }

    if (robot_data.smashing_mode && GetTime() - smashing_start >= 5.0f) {
        robot_data.smashing_mode = false;
    }
}

Rectangle robot_get_rect(const RobotData& robot_data, const MapData& map_data) {
    return {robot_data.pos.x - map_data.GRID_WIDTH / 2.0f, robot_data.pos.y - map_data.GRID_HEIGHT / 2.0f, static_cast<float>(map_data.GRID_WIDTH), static_cast<float>(map_data.GRID_HEIGHT)};
}

const char* print_movement(MovementType move) {
    switch (move) {
        case MovementType::NONE:
            return "none";
        case MovementType::LEFT:
            return "left";
        case MovementType::RIGHT:
            return "right";
        case MovementType::UP:
            return "up";
        case MovementType::DOWN:
            return "down";
    };

    return "";
}
