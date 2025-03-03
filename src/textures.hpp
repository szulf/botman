#pragma once

#include "constants.hpp"
#include "map.hpp"

#include "raylib.h"
#include <string_view>
#include <vector>

struct AnimatedTexture {
public:
    AnimatedTexture(std::string_view texture_path, u8 frame_count, u8 fps) : texture{LoadTexture(texture_path.data())}, frame_count{frame_count}, fps{fps} { }

    ~AnimatedTexture();

    void progress(float dt);
    void reset();

public:
    Texture2D texture{};

    static constexpr u8 width{16};
    static constexpr u8 height{16};

    float accumulator{};
    u8 frame{};

    u8 frame_count{};
    u8 fps{};
};

struct AnimatedTextureList {
public:
    AnimatedTextureList(std::string_view texture_path, u8 frame_count, u8 fps) : texture{LoadTexture(texture_path.data())}, frame_count{frame_count}, fps{fps} { }
    ~AnimatedTextureList();

    void progress(u8 idx, float dt);
    void reset(u8 idx);

    void set_count(u8 count);

public:
    Texture2D texture{};

    static constexpr u8 width{16};
    static constexpr u8 height{16};

    std::vector<float> accumulator{};
    std::vector<u8> frame{};

    u8 frame_count{};
    u8 fps{};
};

struct TexturesType {
public:
    TexturesType(std::string_view hammer_path, std::string_view pellet_path, std::string_view spawner_path, std::string_view wall_path, std::string_view portal_path, std::string_view robot_path, std::string_view robot_hammer_path, std::string_view bug_path, std::string_view start_pos_path, std::string_view empty_path, std::string_view heart_path) : hammer{LoadTexture(hammer_path.data())}, pellet{LoadTexture(pellet_path.data())}, spawner{LoadTexture(spawner_path.data())}, wall{LoadTexture(wall_path.data())}, portal{portal_path, 4, 4}, robot{robot_path, 5, 4}, robot_hammer{robot_hammer_path, 5, 4}, bug{bug_path, 4, 4}, start_pos{LoadTexture(start_pos_path.data())}, empty{LoadTexture(empty_path.data())}, heart{LoadTexture(heart_path.data())} { }

    ~TexturesType();

public:
    Texture2D hammer{};
    Texture2D pellet{};
    Texture2D spawner{};

    struct WallTextureType {
        Texture2D texture{};

        static constexpr u8 width{16};
        static constexpr u8 height{16};
    } wall;

    AnimatedTexture portal;

    AnimatedTexture robot;
    AnimatedTexture robot_hammer;

    AnimatedTextureList bug;

    Texture2D start_pos;
    Texture2D empty;

    Texture2D heart;

    const Texture2D& get_texture_from_tile(Tile tile) const;
};

