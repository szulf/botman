#include "path.hpp"
#include <algorithm>

std::vector<v2> find_path(const v2& start_grid_pos, const v2& end_grid_pos, const MapData& map_data) {
    if (start_grid_pos == end_grid_pos) {
        return {};
    }

    std::vector<Node> closed_set;
    std::vector<Node> open_set{};
    open_set.push_back({start_grid_pos});

    while (!open_set.empty()) {
        Node q{};
        u32 lowest_cost{std::numeric_limits<u32>::max()};
        for (const auto& n : open_set) {
            if (n.f < lowest_cost) {
                lowest_cost = n.f;
                q = n;
            }
        }
        open_set.erase(std::ranges::find(open_set, Node{q}));

        std::vector<Node> successors{
            Node{.pos = {.x = q.pos.x + 1, .y = q.pos.y}, .parent = q.pos},
            Node{.pos = {.x = q.pos.x - 1, .y = q.pos.y}, .parent = q.pos},
            Node{.pos = {.x = q.pos.x, .y = q.pos.y + 1}, .parent = q.pos},
            Node{.pos = {.x = q.pos.x, .y = q.pos.y - 1}, .parent = q.pos},
        };

        if (get_tile(q.pos, map_data) == TILE_PORTAL) {
            successors.push_back(Node{.pos = get_second_portal_pos(q.pos, map_data), .parent = q.pos});
        }

        for (auto& successor : successors) {
            if (successor.pos == end_grid_pos) {
                closed_set.push_back(q);
                Node curr = successor;
                std::vector<v2> path;

                while (curr.pos != start_grid_pos) {
                    path.push_back(curr.pos);

                    for (const auto& n : closed_set) {
                        if (n == Node{.pos = curr.parent}) {
                            curr = n;
                            break;
                        }
                    }
                }

                std::ranges::reverse(path);

                return path;
            }

            if (get_tile(successor.pos, map_data) == TILE_WALL || get_tile(successor.pos, map_data) == TILE_SPAWNER) {
                continue;
            }

            successor.g = q.g + 1;
            successor.h = std::abs(successor.pos.x - end_grid_pos.x) + std::abs(successor.pos.y - end_grid_pos.y) + rand() % 20;
            successor.f = successor.g + successor.h;

            Node it_os{};
            bool found_os{};
            for (const auto& n : open_set) {
                if (n.pos == successor.pos) {
                    it_os = n;
                    found_os = true;
                    break;
                }
            }
            if (found_os) {
                if (it_os.f < successor.f) {
                    continue;
                } else {
                    open_set.erase(std::ranges::find(open_set, it_os));
                }
            }

            Node it_cs{};
            bool found_cs{};
            for (const auto& n : closed_set) {
                if (n.pos == successor.pos) {
                    it_cs = n;
                    found_cs = true;
                    break;
                }
            }
            if (found_cs) {
                if (it_cs.f < successor.f) {
                    continue;
                }
            } else {
                open_set.push_back(successor);
            }
        }

        closed_set.push_back(q);
    };

    return {};
}

v2 find_furthest_grid_pos(const v2& grid_pos, const MapData& map_data) {
    QuadrantType quadrant = static_cast<QuadrantType>(round(grid_pos.x / map_data.WIDTH) + round(grid_pos.y / map_data.HEIGHT) * 2);

    switch (quadrant) {
        case QUAD_TOP_LEFT:
            return {static_cast<float>(map_data.WIDTH - 2), static_cast<float>(map_data.HEIGHT - 2)};
        case QUAD_TOP_RIGHT:
            return {1, static_cast<float>(map_data.HEIGHT - 1)};
        case QUAD_BOTTOM_LEFT:
            return {static_cast<float>(map_data.WIDTH - 2), 1};
        case QUAD_BOTTOM_RIGHT:
            return {1, 1};
    }

    return {0, 0};
}
