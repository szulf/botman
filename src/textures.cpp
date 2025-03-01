#include "textures.hpp"
#include "raylib.h"

AnimatedTexture::~AnimatedTexture() {
    UnloadTexture(texture);
}

void AnimatedTexture::progress(float dt) {
    accumulator += dt;

    frame = fmod(-accumulator * fps, frame_count);
}

AnimatedTextureList::~AnimatedTextureList() {
    UnloadTexture(texture);
}

void AnimatedTextureList::progress(u8 idx, float dt) {
    accumulator[idx] += dt;

    frame[idx] = fmod(-accumulator[idx] * fps, frame_count);
}

void AnimatedTextureList::set_count(u8 count) {
    accumulator.reserve(count);
    frame.reserve(count);
}

TexturesType::~TexturesType() {
    UnloadTexture(hammer);
    UnloadTexture(pellet);
    UnloadTexture(spawner);
    UnloadTexture(wall.texture);
    UnloadTexture(start_pos);
    UnloadTexture(empty);
    UnloadTexture(heart);
}

const Texture2D& TexturesType::get_texture_from_tile(Tile tile) const {
    switch (tile) {
        case Tile::EMPTY:
            return empty;

        case Tile::WALL:
            return wall.texture;

        case Tile::PELLET:
            return pellet;

        case Tile::HAMMER:
            return hammer;

        case Tile::SPAWNER:
            return spawner;

        case Tile::START_POS:
            return start_pos;

        case Tile::PORTAL:
            return portal.texture;
    }

    return empty;
}

