#include "map.hpp"
#include "game.hpp"

#include "raymath.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>

std::string_view print_tile(Tile tile) {
    switch (tile) {
        case Tile::EMPTY:
            return "empty";
        case Tile::WALL:
            return "wall";
        case Tile::PELLET:
            return "pellet";
        case Tile::HAMMER:
            return "hammer";
        case Tile::SPAWNER:
            return "spawner";
        case Tile::START_POS:
            return "start_pos";
        case Tile::PORTAL:
            return "portal";
    }

    return "";
}

MapData::MapData(const v2& map_pos) {
    load(map_pos);
}

v2 MapData::get_grid_from_pos(const v2& _pos) const {
    return {std::floor((_pos.x - pos.x) / GRID_WIDTH), std::floor((_pos.y - pos.y) / GRID_HEIGHT)};
}

v2 MapData::get_pos_from_grid(const v2& grid_pos) const {
    return {(grid_pos.x * GRID_WIDTH) + (GRID_WIDTH / 2.0f) + pos.x, (grid_pos.y * GRID_HEIGHT) + (GRID_HEIGHT / 2.0f) + pos.y};
}

v2 MapData::get_grid_center(const v2& pos) const {
    return get_pos_from_grid(get_grid_from_pos(pos));
}

void MapData::load(const v2& map_pos) {
    pos = map_pos;
    tiles = {WIDTH * HEIGHT, Tile::EMPTY};

    for (u8 i = 0; i < 17; i++) {
        set_tile({static_cast<float>(i), 0}, Tile::WALL);
        set_tile({static_cast<float>(i), 21}, Tile::WALL);
    }
    for (u8 i = 0; i < 21; i++) {
        set_tile({0, static_cast<float>(i)}, Tile::WALL);
        set_tile({16, static_cast<float>(i)}, Tile::WALL);
    }

    std::ifstream file{ROOT_PATH "/map.txt", std::ios::in};
    std::string file_str{(std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>())};

    u8 portal_count = 0;
    for (u32 i = 0; const auto& c : file_str) {
        switch (c) {
            case '#':
            case 'X':
                tiles[i] = Tile::WALL;
                break;

            case 'S':
                tiles[i] = Tile::START_POS;
                start_pos = {static_cast<float>(i % WIDTH), static_cast<float>((i - (i % WIDTH)) / WIDTH)};
                break;

            case 'P':
                tiles[i] = Tile::PELLET;
                pellet_count++;
                break;

            case 'G':
                tiles[i] = Tile::SPAWNER;
                spawner_pos = {static_cast<float>(i % WIDTH), static_cast<float>((i - (i % WIDTH)) / WIDTH)};
                break;

            case 'H':
                tiles[i] = Tile::HAMMER;
                break;

            case '1':
                tiles[i] = Tile::PORTAL;
                portal_pos[portal_count] = {static_cast<float>(i % WIDTH), static_cast<float>((i - (i % WIDTH)) / WIDTH)};
                portal_count++;
                break;

            case ' ':
            case '.':
                break;

            default:
                continue;
        }
        i++;
    }
}

void MapData::save(const char* map_name) const {
    std::string map_file_name = ROOT_PATH "/maps/" + std::string(map_name) + ".txt";
    std::ofstream map_file{map_file_name, std::ios::out};

    for (u16 i = 1; const auto& tile : tiles) {
        switch (tile) {
            case Tile::EMPTY:
                map_file << ".";
                break;
            case Tile::WALL:
                map_file << "#";
                break;
            case Tile::PELLET:
                map_file << "P";
                break;
            case Tile::HAMMER:
                map_file << "H";
                break;
            case Tile::SPAWNER:
                map_file << "G";
                break;
            case Tile::START_POS:
                map_file << "S";
                break;
            case Tile::PORTAL:
                map_file << "1";
                break;
        }
        if (i % WIDTH == 0) {
            map_file << "\n";
        }
        i++;
    }
}

v2 MapData::get_second_portal_pos(const v2& portal_grid_pos) const {
    if (portal_pos[0] == portal_grid_pos) {
        return portal_pos[1];
    } else {
        return portal_pos[0];
    }
}

void MapData::render(TexturesType& textures) const {
    for (u32 i = 0; i < WIDTH; i++) {
        for (u32 j = 0; j < HEIGHT; j++) {
            Tile tile = get_tile({static_cast<float>(i), static_cast<float>(j)});
            switch (tile) {
                case Tile::WALL: {
                    calc_wall_texture({static_cast<float>(i), static_cast<float>(j)});
                    const v2 pos = get_pos_from_grid({static_cast<float>(i), static_cast<float>(j)});
                    const Rectangle rect = {pos.x, pos.y, static_cast<float>(GRID_WIDTH), static_cast<float>(GRID_HEIGHT)};
                    DrawTexturePro(textures.wall.texture, {static_cast<float>(textures.wall.width * calc_wall_texture({static_cast<float>(i), static_cast<float>(j)})), 0, static_cast<float>(textures.wall.width), static_cast<float>(textures.wall.height)}, rect, {GRID_WIDTH / 2.0f, GRID_HEIGHT / 2.0f}, 0.0f, WHITE);
                    break;
                }

                case Tile::PELLET: {
                    const v2 pos = get_pos_from_grid({static_cast<float>(i), static_cast<float>(j)});
                    const Rectangle rect = {pos.x, pos.y, static_cast<float>(GRID_WIDTH), static_cast<float>(GRID_HEIGHT)};
                    DrawTexturePro(textures.pellet, {0, 0, static_cast<float>(textures.pellet.width), static_cast<float>(textures.pellet.height)}, rect, {GRID_WIDTH / 2.0f, GRID_HEIGHT / 2.0f}, 0.0f, WHITE);
                    break;
                }

                case Tile::HAMMER: {
                    const v2 pos = get_pos_from_grid({static_cast<float>(i), static_cast<float>(j)});
                    const Rectangle rect = {pos.x, pos.y, static_cast<float>(GRID_WIDTH), static_cast<float>(GRID_HEIGHT)};
                    DrawTexturePro(textures.hammer, {0, 0, static_cast<float>(textures.hammer.width), static_cast<float>(textures.hammer.height)}, rect, {GRID_WIDTH / 2.0f, GRID_HEIGHT / 2.0f}, 0.0f, WHITE);
                    break;
                }

                case Tile::SPAWNER: {
                    const v2 pos = get_pos_from_grid({static_cast<float>(i), static_cast<float>(j)});
                    const Rectangle rect = {pos.x, pos.y, static_cast<float>(GRID_WIDTH), static_cast<float>(GRID_HEIGHT)};
                    DrawTexturePro(textures.spawner, {0, 0, static_cast<float>(textures.spawner.width), static_cast<float>(textures.spawner.height)}, rect, {GRID_WIDTH / 2.0f, GRID_HEIGHT / 2.0f}, 0.0f, WHITE);
                    break;
                }

                case Tile::PORTAL: {
                    const v2 pos = get_pos_from_grid({static_cast<float>(i), static_cast<float>(j)});
                    const Rectangle rect = {pos.x, pos.y, static_cast<float>(GRID_WIDTH), static_cast<float>(GRID_HEIGHT)};
                    DrawTexturePro(textures.portal.texture, {static_cast<float>(textures.portal.width * textures.portal.frame), 0, static_cast<float>(textures.portal.width), static_cast<float>(textures.portal.height)}, rect, {GRID_WIDTH / 2.0f, GRID_HEIGHT / 2.0f}, 0.0f, WHITE);
                    break;
                }

                default:
                    break;
            }
        }
    }
}

bool MapData::in_about_center(const v2& pos) const {
    auto center_pos = get_grid_center(pos);
    return (center_pos.x - 4 <= pos.x && center_pos.x + 4 >= pos.x) && (center_pos.y - 4 <= pos.y && center_pos.y + 4 >= pos.y);
}

u8 MapData::calc_wall_texture(const v2& grid_pos) const {
    static constexpr v2 directions[] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
    u8 connections{};

    for (u8 dir_idx = 0; const auto& dir : directions) {
        if (grid_pos.x + dir.x >= WIDTH || grid_pos.x + dir.x < 0 || grid_pos.y + dir.y >= HEIGHT || grid_pos.y + dir.y < 0) {
            dir_idx++;
            continue;
        }

        if (get_tile(grid_pos + dir) == Tile::WALL) {
            connections += 1 << dir_idx;
        }

        dir_idx++;
    }

    return connections;
}

