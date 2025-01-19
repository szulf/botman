#include "texture_manager.hpp"

#include "raylib.h"

namespace botman
{

TextureManager::TextureManager(std::span<std::string> texture_ids)
{
    for (const auto& t_id : texture_ids)
    {
        auto texture = ::LoadTexture((ROOT_PATH "/assets/" + t_id).c_str());
        m_cache[t_id] = texture;
    }
}

TextureManager::~TextureManager()
{
    for (const auto& [_, texture] : m_cache)
    {
        ::UnloadTexture(texture);
    }
}

auto TextureManager::get_texture(const std::string& texture_id) -> const Texture2D&
{
    if (!m_cache.contains(texture_id))
    {
        auto texture = ::LoadTexture((ROOT_PATH "/assets/" + texture_id).c_str());
        m_cache[texture_id] = texture;
    }

    return m_cache[texture_id];
}

auto TextureManager::load_texture(const std::string& texture_id) -> void
{
    if (!m_cache.contains(texture_id))
    {
        auto texture = ::LoadTexture((ROOT_PATH "/assets/" + texture_id).c_str());
        m_cache[texture_id] = texture;
    }
}

}
