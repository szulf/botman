#pragma once

#include "misc.hpp"
#include <array>
#include <cstdint>
#include <vector>

struct GameState
{
    static constexpr uint16_t WIDTH{1920};
    static constexpr uint16_t HEIGHT{1080};
    static constexpr uint16_t FPS{0};

    static constexpr uint8_t GRID_WIDTH{32};
    static constexpr uint8_t GRID_HEIGHT{32};

    static constexpr uint8_t MAP_WIDTH{17};
    static constexpr uint8_t MAP_HEIGHT{22};

    static constexpr uint8_t PELLET_WIDTH{8};
    static constexpr uint8_t PELLET_HEIGHT{8};

    static constexpr uint8_t EATING_BALL_WIDTH{12};
    static constexpr uint8_t EATING_BALL_HEIGHT{12};

    static constexpr float MOVE_SPEED{130.0f};

    static constexpr Vec2 MAP_POS{10, 3};

    Vec2 start_pos{10, 3};
    Vec2 spawner_pos{10, 4};

    int32_t score{0};
    float delta_time{0.0f};
    bool freeze{false};
    float smashing_mode{0.0f};

    std::array<std::array<Tile, HEIGHT / GRID_HEIGHT>, WIDTH / GRID_WIDTH> map{};
    std::vector<Vec2> walls{};
    std::vector<Vec2> pellets{};
    std::vector<Vec2> hammers{};
};

extern GameState game_state;

