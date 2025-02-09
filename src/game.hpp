#pragma once

#include "constants.hpp"

enum class GameStateType : u8 {
    START_SCREEN,
    EDIT_MODE,
    SETTINGS,
    RUNNING,
    WON,
    LOST,
};

struct GameData {
    GameStateType state{};
};
