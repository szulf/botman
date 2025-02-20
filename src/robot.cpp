#include "robot.hpp"
#include "map.hpp"
#include "game.hpp"
#include "raylib.h"
#include "raymath.h"

std::string_view print_movement(Movement move) {
    switch (move) {
        case Movement::NONE:
            return "none";
        case Movement::LEFT:
            return "left";
        case Movement::RIGHT:
            return "right";
        case Movement::UP:
            return "up";
        case Movement::DOWN:
            return "down";
    };

    return "";
}

void RobotData::render(const MapData& map_data, const TexturesType& textures) const {
    DrawTexturePro(textures.robot.texture, {static_cast<float>(textures.robot.width * textures.robot.frame), 0, static_cast<float>(textures.robot.width * static_cast<i8>(flip)), static_cast<float>(textures.robot.height)}, {pos.x, pos.y, static_cast<float>(map_data.GRID_WIDTH), static_cast<float>(map_data.GRID_HEIGHT)}, {map_data.GRID_WIDTH / 2.0f, map_data.GRID_HEIGHT / 2.0f}, 0.0f, WHITE);
}

void RobotData::move(Movement move, float dt, const MapData& map_data) {
    switch (move) {
        case Movement::UP:
            if (movement == v2{0, 1})
            {
                movement = {0, -1};
            }
            else
            {
                next_move = move;
                time_between_moves = GetTime();
            }
            break;

        case Movement::DOWN:
            if (movement == v2{0, -1})
            {
                movement = {0, 1};
            }
            else
            {
                next_move = move;
                time_between_moves = GetTime();
            }
            break;

        case Movement::LEFT:
            if (movement == v2{1, 0})
            {
                movement = {-1, 0};
                flip = Flip::LEFT;
            }
            else
            {
                next_move = move;
                time_between_moves = GetTime();
            }
            break;

        case Movement::RIGHT:
            if (movement == v2{-1, 0})
            {
                movement = {1, 0};
                flip = Flip::RIGHT;
            }
            else
            {
                next_move = move;
                time_between_moves = GetTime();
            }
            break;

        case Movement::NONE:
            break;
    }

    auto grid_pos = map_data.get_grid_from_pos(pos);
    if (map_data.in_about_center(pos) && next_move != Movement::NONE) {
        static bool first = true;
        if (GetTime() - time_between_moves >= 0.35f && !first)
        {
            next_move = Movement::NONE;
        }
        first = false;

        switch (next_move) {
            case Movement::LEFT: {
                if (movement == v2{-1, 0}) {
                    break;
                }
                v2 next_movement = {-1, 0};
                auto next_pos = grid_pos + next_movement;
                auto next_tile = map_data.get_tile(next_pos);
                if (next_tile != Tile::WALL && next_tile != Tile::SPAWNER) {
                    movement = next_movement;
                    pos = map_data.get_grid_center(pos);
                    next_move = Movement::NONE;

                    flip = Flip::LEFT;
                }
                break;
            }

            case Movement::RIGHT: {
                if (movement == v2{1, 0}) {
                    break;
                }
                v2 next_movement = {1, 0};
                auto next_pos = grid_pos + next_movement;
                auto next_tile = map_data.get_tile(next_pos);
                if (next_tile != Tile::WALL && next_tile != Tile::SPAWNER) {
                    movement = next_movement;
                    pos = map_data.get_grid_center(pos);
                    next_move = Movement::NONE;

                    flip = Flip::RIGHT;
                }
                break;
            }

            case Movement::UP: {
                if (movement == v2{0, -1}) {
                    break;
                }
                v2 next_movement = {0, -1};
                auto next_pos = grid_pos + next_movement;
                auto next_tile = map_data.get_tile(next_pos);
                if (next_tile != Tile::WALL && next_tile != Tile::SPAWNER) {
                    movement = next_movement;
                    pos = map_data.get_grid_center(pos);
                    next_move = Movement::NONE;
                }
                break;
            }

            case Movement::DOWN: {
                if (movement == v2{0, 1}) {
                    break;
                }
                v2 next_movement = {0, 1};
                auto next_pos = grid_pos + next_movement;
                auto next_tile = map_data.get_tile(next_pos);
                if (next_tile != Tile::WALL && next_tile != Tile::SPAWNER) {
                    movement = next_movement;
                    pos = map_data.get_grid_center(pos);
                    next_move = Movement::NONE;
                }
                break;
            }

            case Movement::NONE:
                break;
        }
    }

    auto next_grid_pos = grid_pos + movement;
    auto next_pos = map_data.get_pos_from_grid(next_grid_pos);
    if (
            (map_data.get_tile(next_grid_pos) == Tile::WALL || map_data.get_tile(next_grid_pos) == Tile::SPAWNER) &&
            CheckCollisionRecs(
                {next_pos.x - map_data.GRID_WIDTH / 2.0f, next_pos.y - map_data.GRID_HEIGHT / 2.0f, static_cast<float>(map_data.GRID_WIDTH), static_cast<float>(map_data.GRID_HEIGHT)},
                collision_rect(map_data)
            )
        ) {
        movement = {0, 0};
        pos = map_data.get_grid_center(pos);
    } else {
        if (map_data.get_tile(map_data.get_grid_from_pos(pos)) == Tile::PORTAL) {
            if (!teleported && map_data.in_about_center(pos)) {
                pos = map_data.get_pos_from_grid(map_data.get_second_portal_pos(grid_pos));
                teleported = true;
            }
        } else {
            teleported = false;
        }

        pos -= movement * dt * MOVEMENT_SPEED;
    }
}

void RobotData::collect(MapData& map_data, GameData& game_data) {
    auto grid_pos = map_data.get_grid_from_pos(pos);

    if (map_data.get_tile(grid_pos) == Tile::PELLET && CheckCollisionRecs({pos.x - (map_data.GRID_WIDTH / 8.0f), pos.y - (map_data.GRID_HEIGHT / 8.0f), map_data.GRID_WIDTH / 4.0f, map_data.GRID_HEIGHT / 4.0f}, collision_rect(map_data))) {
        map_data.set_tile(grid_pos, Tile::EMPTY);
        map_data.score += 10;
        map_data.pellet_count--;
        if (map_data.pellet_count == 0) {
            game_data.change_state(GameState::WON);
        }
    }

    static float smashing_start{};
    if (map_data.get_tile(grid_pos) == Tile::HAMMER && CheckCollisionRecs({pos.x - (map_data.GRID_WIDTH / 4.0f), pos.y - (map_data.GRID_HEIGHT / 4.0f), map_data.GRID_WIDTH / 2.0f, map_data.GRID_HEIGHT / 2.0f}, collision_rect(map_data))) {
        map_data.set_tile(grid_pos, Tile::EMPTY);
        smashing_mode = true;
        smashing_start = GetTime();
    }

    if (smashing_mode && GetTime() - smashing_start >= 5.0f) {
        smashing_mode = false;
    }
}

Rectangle RobotData::collision_rect(const MapData& map_data) const {
    return {
        .x = pos.x - map_data.GRID_WIDTH / 2.0f + (map_data.GRID_WIDTH * 0.125f),
        .y = (pos.y - map_data.GRID_HEIGHT / 2.0f) + (map_data.GRID_HEIGHT * 0.125f),
        .width = static_cast<float>(map_data.GRID_WIDTH - (map_data.GRID_WIDTH * 0.25f)),
        .height = static_cast<float>(map_data.GRID_HEIGHT - (map_data.GRID_HEIGHT * 0.125f))
    };
}

