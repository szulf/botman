#include "map.hpp"

#include "raymath.h"
#include <fstream>
#include <string>
#include <cmath>
#include <string.h>

MapData load_map(const v2& map_pos) {
    MapData map_data{
        .pos = map_pos,
        .tiles = {MapData::WIDTH * MapData::HEIGHT, TILE_EMPTY},
    };

    for (u8 i = 0; i < 17; i++) {
        set_tile({static_cast<float>(i), 0}, TILE_WALL, map_data);
        set_tile({static_cast<float>(i), 21}, TILE_WALL, map_data);
    }
    for (u8 i = 0; i < 21; i++) {
        set_tile({0, static_cast<float>(i)}, TILE_WALL, map_data);
        set_tile({16, static_cast<float>(i)}, TILE_WALL, map_data);
    }

    std::ifstream file{ROOT_PATH "/map.txt", std::ios::in | std::ios::binary};
    std::string file_str{(std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>())};

    u8 portal_count = 0;
    for (u32 i = 0; const auto& c : file_str) {
        switch (c) {
            case '#':
            case 'X':
                map_data.tiles[i] = TILE_WALL;
                break;

            case 'S':
                map_data.tiles[i] = TILE_START_POS;
                map_data.start_pos = {static_cast<float>(i % map_data.WIDTH), static_cast<float>((i - (i % map_data.WIDTH)) / map_data.WIDTH)};
                break;

            case 'P':
                map_data.tiles[i] = TILE_PELLET;
                map_data.pellet_count++;
                break;

            case 'G':
                map_data.tiles[i] = TILE_SPAWNER;
                map_data.spawner_pos = {static_cast<float>(i % map_data.WIDTH), static_cast<float>((i - (i % map_data.WIDTH)) / map_data.WIDTH)};
                break;

            case 'H':
                map_data.tiles[i] = TILE_HAMMER;
                break;

            case '1':
                map_data.tiles[i] = TILE_PORTAL;
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

void save_map(const char* map_name, const MapData& map_data) {
    char* buf = (char*) malloc(strlen(map_name) + strlen(ROOT_PATH "/maps/") + 5);
    strcpy(buf, ROOT_PATH "/maps/");
    strcat(buf, map_name);
    strcat(buf, ".txt");

    FILE* map_file = fopen(buf, "w");

    for (u16 i = 1; const auto& tile : map_data.tiles) {
        switch (tile) {
            case TILE_EMPTY:
                fprintf(map_file, ".");
                break;
            case TILE_WALL:
                fprintf(map_file, "#");
                break;
            case TILE_PELLET:
                fprintf(map_file, "P");
                break;
            case TILE_HAMMER:
                fprintf(map_file, "H");
                break;
            case TILE_SPAWNER:
                fprintf(map_file, "G");
                break;
            case TILE_START_POS:
                fprintf(map_file, "S");
                break;
            case TILE_PORTAL:
                fprintf(map_file, "1");
                break;
        }
        if (i % map_data.WIDTH == 0) {
            fprintf(map_file, "\n");
        }
        i++;
    }

    fclose(map_file);
    free((void*) buf);
}

v2 get_grid_from_pos(const v2& pos, const MapData& map_data) {
    return {std::floor((pos.x - map_data.pos.x) / map_data.GRID_WIDTH), std::floor((pos.y - map_data.pos.y) / map_data.GRID_HEIGHT)};
}

v2 get_pos_from_grid(const v2& grid_pos, const MapData& map_data) {
    return {(grid_pos.x * map_data.GRID_WIDTH) + (map_data.GRID_WIDTH / 2.0f) + map_data.pos.x, (grid_pos.y * map_data.GRID_HEIGHT) + (map_data.GRID_HEIGHT / 2.0f) + map_data.pos.y};
}

void render_map(const MapData& map_data, const Texture2D& hammer_texture, const Texture2D& portal_texture, const Texture2D& pellet_texture) {
    for (u32 i = 0; i < map_data.WIDTH; i++) {
        for (u32 j = 0; j < map_data.HEIGHT; j++) {
            TileType tile = get_tile({static_cast<float>(i), static_cast<float>(j)}, map_data);
            switch (tile) {
                case TILE_WALL: {
                    const v2 pos = get_pos_from_grid({static_cast<float>(i), static_cast<float>(j)}, map_data);
                    const Rectangle rect = {pos.x, pos.y, static_cast<float>(map_data.GRID_WIDTH), static_cast<float>(map_data.GRID_HEIGHT)};
                    DrawRectanglePro(rect, {map_data.GRID_WIDTH / 2.0f, map_data.GRID_HEIGHT / 2.0f}, 0.0f, BLACK);
                    break;
                }

                case TILE_PELLET: {
                    const v2 pos = get_pos_from_grid({static_cast<float>(i), static_cast<float>(j)}, map_data);
                    const Rectangle rect = {pos.x, pos.y, (float) map_data.GRID_WIDTH, (float) map_data.GRID_HEIGHT};
                    DrawTexturePro(pellet_texture, {0, 0, (float) hammer_texture.width, (float) hammer_texture.height}, rect, {map_data.GRID_WIDTH / 2.0f, map_data.GRID_HEIGHT / 2.0f}, 0.0f, WHITE);
                    break;
                }

                case TILE_HAMMER: {
                    const v2 pos = get_pos_from_grid({static_cast<float>(i), static_cast<float>(j)}, map_data);
                    const Rectangle rect = {pos.x, pos.y, (float) map_data.GRID_WIDTH, (float) map_data.GRID_HEIGHT};
                    DrawTexturePro(hammer_texture, {0, 0, (float) hammer_texture.width, (float) hammer_texture.height}, rect, {map_data.GRID_WIDTH / 2.0f, map_data.GRID_HEIGHT / 2.0f}, 0.0f, WHITE);
                    break;
                }

                case TILE_SPAWNER: {
                    const v2 pos = get_pos_from_grid(map_data.spawner_pos, map_data);
                    const Rectangle rect = {pos.x, pos.y, static_cast<float>(map_data.GRID_WIDTH), static_cast<float>(map_data.GRID_HEIGHT)};
                    DrawRectanglePro(rect, {map_data.GRID_WIDTH / 2.0f, map_data.GRID_HEIGHT / 2.0f}, 0.0f, RED);
                    break;
                }

                case TILE_PORTAL: {
                    const v2 pos = get_pos_from_grid({static_cast<float>(i), static_cast<float>(j)}, map_data);
                    const Rectangle rect = {pos.x, pos.y, (float) map_data.GRID_WIDTH, (float) map_data.GRID_HEIGHT};
                    DrawTexturePro(portal_texture, {0, 0, (float) hammer_texture.width, (float) hammer_texture.height}, rect, {map_data.GRID_WIDTH / 2.0f, map_data.GRID_HEIGHT / 2.0f}, 0.0f, WHITE);
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
        case TILE_EMPTY:
            return "empty";
        case TILE_WALL:
            return "wall";
        case TILE_PELLET:
            return "pellet";
        case TILE_HAMMER:
            return "hammer";
        case TILE_SPAWNER:
            return "spawner";
        case TILE_START_POS:
            return "start_pos";
        case TILE_PORTAL:
            return "portal";
    }

    return "";
}
