#pragma once

#include "constants.hpp"

#include <cmath>
#include <string_view>
#include <vector>

// Gotta love circular dependencies
struct TexturesType;

enum class Tile : u8 {
    EMPTY,
    WALL,
    PELLET,
    HAMMER,
    SPAWNER,
    START_POS,

    // TODO
    // for now only allowing two portals that link to each other
    // maybe change that in the future
    PORTAL,
};

struct MapData {
public:
    MapData() = default;
    MapData(const v2& map_pos);

    inline Tile get_tile(const v2& pos) const {
        return tiles[pos.x + WIDTH * pos.y];
    }

    inline Tile get_tile(u32 x, u32 y) const {
        return tiles[x + WIDTH * y];
    }

    inline void set_tile(v2 pos, Tile tile) {
        tiles[pos.x + WIDTH * pos.y] = tile;
    }

    inline bool in_range(const v2& grid_pos) const {
        return (grid_pos.x > 0) && (grid_pos.x < WIDTH - 1) && (grid_pos.y > 0) && (grid_pos.y < HEIGHT - 1);
    }

    v2 get_grid_from_pos(const v2& _pos) const;
    v2 get_pos_from_grid(const v2& grid_pos) const;
    v2 get_grid_center(const v2& pos) const;

    void load(const v2& map_pos);
    void save(const char* map_name) const;

    v2 get_second_portal_pos(const v2& portal_pos) const;

    // TODO
    // dt should not be here
    void render(TexturesType& textures, float dt) const;

    bool in_about_center(const v2& pos) const;

private:
    u8 calc_wall_texture(const v2& grid_pos) const;

public:
    static constexpr u32 WIDTH{17};
    static constexpr u32 HEIGHT{22};

    static constexpr u32 GRID_WIDTH{32};
    static constexpr u32 GRID_HEIGHT{32};

    v2 pos{};

    std::vector<Tile> tiles;
    v2 start_pos{};
    v2 spawner_pos{};
    v2 portal_pos[2]{};

    u32 score{};
    u16 pellet_count{};

};

std::string_view print_tile(Tile tile);

