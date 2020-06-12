#pragma once

#include <fstream>
#include <sstream>
#include <vector>

#include "game_object.hpp"
#include "resource_manager.hpp"

class GameLevel {
public:
    GameLevel() { }

    void load(ResourceManager& resourceManager, const std::string& file, size_t levelWidth, size_t levelHeight)
    {
        m_bricks.clear();

        unsigned int tileCode;
        GameLevel level;
        std::string line;
        std::ifstream fstream(file.c_str());
        std::vector<std::vector<size_t>> tileData;

        if (fstream) {
            while (std::getline(fstream, line)) {
                std::istringstream sstream(line);
                std::vector<size_t> row;

                while (sstream >> tileCode)
                    row.push_back(tileCode);

                tileData.push_back(row);
            }

            if (!tileData.empty())
                init(resourceManager, tileData, levelWidth, levelHeight);
        }
    }

    void draw(SpriteRenderer& renderer)
    {
        for (auto& tile : m_bricks) {
            if (!tile.getIsDestroyed())
                tile.draw(renderer);
        }
    }

    bool isCompleted()
    {
        for (auto& tile : m_bricks) {
            if (!tile.getIsSolid() && !tile.getIsDestroyed())
                return false;
        }

        return true;
    }

private:
    std::vector<GameObject> m_bricks;

    void init(ResourceManager& resourceManager, std::vector<std::vector<size_t>> tileData, size_t levelWidth, size_t levelHeight)
    {
        // calculate dimensions
        size_t height { tileData.size() };
        size_t width { tileData[0].size() };
        float unitWidth { levelWidth / static_cast<float>(width) };
        float unitHeight { levelHeight / static_cast<float>(height) };

        for (size_t y { 0 }; y < height; ++y) {
            for (size_t x { 0 }; x < width; ++x) {
                if (tileData[y][x] == 1) {
                    glm::vec2 pos { unitWidth * x, unitHeight * y };
                    glm::vec2 size { unitWidth, unitHeight };
                    Texture2D texture { resourceManager.getTexture("block_solid") };

                    GameObject obj { pos, size, texture, glm::vec3(0.8f, 0.8f, 0.7f) };
                    obj.setIsSolid(true);
                    m_bricks.push_back(obj);
                } else if (tileData[y][x] > 1) {
                    glm::vec2 pos { unitWidth * x, unitHeight * y };
                    glm::vec2 size { unitWidth, unitHeight };
                    glm::vec3 color { glm::vec3(1) };
                    Texture2D texture { resourceManager.getTexture("block") };

                    if (tileData[y][x] == 2)
                        color = glm::vec3(0.2f, 0.6f, 1.0f);
                    else if (tileData[y][x] == 3)
                        color = glm::vec3(0.0f, 0.7f, 0.0f);
                    else if (tileData[y][x] == 4)
                        color = glm::vec3(0.8f, 0.8f, 0.4f);
                    else if (tileData[y][x] == 5)
                        color = glm::vec3(1.0f, 0.5f, 0.0f);

                    m_bricks.push_back(GameObject { pos, size, texture, color });
                }
            }
        }
    }
};
