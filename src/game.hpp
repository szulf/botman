#pragma once

#include "constants.hpp"

enum GameStateType : u8 {
    GAME_START_SCREEN,
    GAME_EDIT_MODE,
    GAME_SETTINGS,
    GAME_RUNNING,
    GAME_WON,
    GAME_LOST,
};

struct GameData {
    GameStateType state{};
};
