#include "map.hpp"

#include "raymath.h"
#include <fstream>
#include <string>
#include <cmath>

MapData load_map(const v2& map_pos) {
    MapData map_data{
        .pos = map_pos,
        .tiles = {MapData::WIDTH * MapData::HEIGHT, TileType::EMPTY},
    };

    for (u8 i = 0; i < 17; i++) {
        set_tile({static_cast<float>(i), 0}, TileType::WALL, map_data);
        set_tile({static_cast<float>(i), 21}, TileType::WALL, map_data);
    }
    for (u8 i = 0; i < 21; i++) {
        set_tile({0, static_cast<float>(i)}, TileType::WALL, map_data);
        set_tile({16, static_cast<float>(i)}, TileType::WALL, map_data);
    }

    std::ifstream file{ROOT_PATH "/map.txt", std::ios::in | std::ios::binary};
    std::string file_str{(std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>())};

    u8 portal_count = 0;
    for (u32 i = 0; const auto& c : file_str) {
        switch (c) {
            case '#':
            case 'X':
                map_data.tiles[i] = TileType::WALL;
                break;

            case 'S':
                map_data.tiles[i] = TileType::START_POS;
                map_data.start_pos = {static_cast<float>(i % map_data.WIDTH), static_cast<float>((i - (i % map_data.WIDTH)) / map_data.WIDTH)};
                break;

            case 'P':
                map_data.tiles[i] = TileType::PELLET;
                map_data.pellet_count++;
                break;

            case 'G':
                map_data.tiles[i] = TileType::SPAWNER;
                map_data.spawner_pos = {static_cast<float>(i % map_data.WIDTH), static_cast<float>((i - (i % map_data.WIDTH)) / map_data.WIDTH)};
                break;

            case 'H':
                map_data.tiles[i] = TileType::HAMMER;
                break;

            case '1':
                map_data.tiles[i] = TileType::PORTAL;
                map_data.portal_pos[portal_count] = {static_cast<float>(i % map_data.WIDTH), static_cast<float>((i - (i % map_data.WIDTH)) / map_data.WIDTH)};
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

    return map_data;
}

v2 get_grid_from_pos(const v2& pos, const MapData& map_data) {
    return {std::floor((pos.x - map_data.pos.x) / map_data.GRID_WIDTH), std::floor((pos.y - map_data.pos.y) / map_data.GRID_HEIGHT)};
}

v2 get_pos_from_grid(const v2& grid_pos, const MapData& map_data) {
    return {(grid_pos.x * map_data.GRID_WIDTH) + (map_data.GRID_WIDTH / 2.0f) + map_data.pos.x, (grid_pos.y * map_data.GRID_HEIGHT) + (map_data.GRID_HEIGHT / 2.0f) + map_data.pos.y};
}

void render_map(const MapData& map_data, const Texture2D& hammer_texture) {
    for (u32 i = 0; i < map_data.WIDTH; i++) {
        for (u32 j = 0; j < map_data.HEIGHT; j++) {
            TileType tile = get_tile({static_cast<float>(i), static_cast<float>(j)}, map_data);
            switch (tile) {
                case TileType::WALL: {
                    const v2 pos = get_pos_from_grid({static_cast<float>(i), static_cast<float>(j)}, map_data);
                    const Rectangle rect = {pos.x, pos.y, static_cast<float>(map_data.GRID_WIDTH), static_cast<float>(map_data.GRID_HEIGHT)};
                    DrawRectanglePro(rect, {map_data.GRID_WIDTH / 2.0f, map_data.GRID_HEIGHT / 2.0f}, 0.0f, BLACK);
                    break;
                }

                case TileType::PELLET: {
                    const v2 pos = get_pos_from_grid({static_cast<float>(i), static_cast<float>(j)}, map_data);
                    const Rectangle rect = {pos.x, pos.y, map_data.GRID_WIDTH / 4.0f, map_data.GRID_HEIGHT / 4.0f};
                    DrawRectanglePro(rect, {map_data.GRID_WIDTH / 4.0f / 2.0f, map_data.GRID_HEIGHT / 4.0f / 2.0f}, 0.0f, BLACK);
                    break;
                }

                case TileType::HAMMER: {
                    const v2 pos = get_pos_from_grid({static_cast<float>(i), static_cast<float>(j)}, map_data);
                    const Rectangle rect = {pos.x, pos.y, (float) map_data.GRID_WIDTH, (float) map_data.GRID_HEIGHT};
                    DrawTexturePro(hammer_texture, {0, 0, (float) hammer_texture.width, (float) hammer_texture.height}, rect, {map_data.GRID_WIDTH / 2.0f, map_data.GRID_HEIGHT / 2.0f}, 0.0f, WHITE);
                    break;
                }

                case TileType::SPAWNER: {
                    const v2 pos = get_pos_from_grid(map_data.spawner_pos, map_data);
                    const Rectangle rect = {pos.x, pos.y, static_cast<float>(map_data.GRID_WIDTH), static_cast<float>(map_data.GRID_HEIGHT)};
                    DrawRectanglePro(rect, {map_data.GRID_WIDTH / 2.0f, map_data.GRID_HEIGHT / 2.0f}, 0.0f, RED);
                    break;
                }

                case TileType::PORTAL: {
                    const v2 pos = get_pos_from_grid({static_cast<float>(i), static_cast<float>(j)}, map_data);
                    const Rectangle rect = {pos.x, pos.y, static_cast<float>(map_data.GRID_WIDTH), static_cast<float>(map_data.GRID_HEIGHT)};
                    DrawRectanglePro(rect, {map_data.GRID_WIDTH / 2.0f, map_data.GRID_HEIGHT / 2.0f}, 0.0f, PURPLE);
                    break;
                }

                default:
                    break;
            }
        }
    }
}

v2 get_second_portal_pos(const v2& portal_pos, const MapData& map_data) {
    if (map_data.portal_pos[0] == portal_pos) {
        return map_data.portal_pos[1];
    } else {
        return map_data.portal_pos[0];
    }
}

const char* print_tile(TileType tile) {
    switch (tile) {
        case TileType::EMPTY:
            return "empty";
        case TileType::WALL:
            return "wall";
        case TileType::PELLET:
            return "pellet";
        case TileType::HAMMER:
            return "hammer";
        case TileType::SPAWNER:
            return "spawner";
        case TileType::START_POS:
            return "start_pos";
        case TileType::PORTAL:
            return "portal";
    }

    return "";
}
