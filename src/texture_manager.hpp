#pragma once

#include "raylib.h"
#include <span>
#include <string>
#include <unordered_map>

namespace botman
{

class TextureManager
{
public:
    TextureManager() {}
    ~TextureManager();

    auto get_texture(const std::string& texture_id) -> const Texture2D&;
    auto load_texture(const std::string& texture_id) -> void;
    auto load_texture(std::span<std::string> texture_ids) -> void;

private:
    std::unordered_map<std::string, Texture2D> m_cache{};

};

}
