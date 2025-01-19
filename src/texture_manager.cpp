#include "texture_manager.hpp"

#include "raylib.h"

namespace botman
{

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

auto TextureManager::load_texture(std::span<std::string> texture_ids) -> void
{
    for (const auto& texture_id : texture_ids)
    {
        if (!m_cache.contains(texture_id))
        {
            auto texture = ::LoadTexture((ROOT_PATH "/assets/" + texture_id).c_str());
            m_cache[texture_id] = texture;
        }
    }
}

}
