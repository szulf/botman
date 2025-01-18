#pragma once

#include "raylib.h"
#include <cstdint>
#include <string_view>

using i8 = int8_t;
using u8 = uint8_t;
using i16 = int16_t;
using u16 = uint16_t;
using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;

using vec2 = Vector2;

namespace botman
{
    namespace constants
    {
        constexpr std::string_view GAME_NAME{"botman"};
        constexpr u32 WIDTH{800};
        constexpr u32 HEIGHT{600};

        constexpr u8 GRID_WIDTH{32};
        constexpr u8 GRID_HEIGHT{32};

        constexpr u8 MAP_WIDTH{17};
        constexpr u8 MAP_HEIGHT{22};

        constexpr u8 PELLET_WIDTH{GRID_WIDTH / 4};
        constexpr u8 PELLET_HEIGHT{GRID_HEIGHT / 4};

        constexpr u8 HAMMER_WIDTH{GRID_WIDTH / 2};
        constexpr u8 HAMMER_HEIGHT{GRID_HEIGHT / 2};
    }
}
