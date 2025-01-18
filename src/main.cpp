#include "game.hpp"

#include <exception>
#include <print>

// TODO
// switch ROOT_PATH macro to a configure file with a constexpr std::filesystem::path

auto main() -> int
{
    try
    {
        botman::Game game{};
        game.run();
    }
    catch (std::exception e)
    {
        std::println("{}", e.what());
        return 1;
    }

    return 0;
}
