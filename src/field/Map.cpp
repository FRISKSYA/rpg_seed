#include "field/Map.h"
#include "system/Renderer.h"
#include "system/ResourceManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

const Tile Map::defaultTile_ = Tile::wall();

Map::Map() : width_(0), height_(0), spawnX_(1), spawnY_(1) {}

bool Map::loadFromCSV(const std::string& path) {
    // Security: Validate path to prevent directory traversal attacks
    if (path.find("..") != std::string::npos ||
        path.find('/') == 0 ||
        path.find('\\') != std::string::npos) {
        std::cerr << "Invalid map path: path traversal not allowed" << std::endl;
        return false;
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open map file" << std::endl;
        return false;
    }

    tiles_.clear();
    std::vector<std::vector<int>> tempData;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::vector<int> row;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ',')) {
            try {
                int tileId = std::stoi(cell);
                // Validate tile ID is within valid TileType range
                if (tileId < Constants::MIN_TILE_ID || tileId > Constants::MAX_TILE_ID) {
                    std::cerr << "Tile ID out of range, defaulting to grass" << std::endl;
                    tileId = 0;
                }
                row.push_back(tileId);
            } catch (const std::exception&) {
                std::cerr << "Invalid tile value in map, defaulting to grass" << std::endl;
                row.push_back(0);  // Default to grass
            }
        }

        if (!row.empty()) {
            tempData.push_back(row);
        }
    }

    if (tempData.empty()) {
        std::cerr << "Map file is empty" << std::endl;
        return false;
    }

    height_ = static_cast<int>(tempData.size());
    width_ = static_cast<int>(tempData[0].size());

    // Convert to Tile objects
    tiles_.reserve(width_ * height_);
    for (const auto& row : tempData) {
        for (int id : row) {
            tiles_.push_back(Tile::fromId(id));
        }
        // Pad rows if necessary
        for (size_t i = row.size(); i < static_cast<size_t>(width_); ++i) {
            tiles_.push_back(Tile::grass());
        }
    }

    return true;
}

bool Map::loadTileSet(ResourceManager& resourceManager, const std::string& path) {
    return tileSet_.load(resourceManager, path);
}

void Map::render(Renderer& renderer, int cameraX, int cameraY) const {
    if (!tileSet_.getTexture()) return;

    int tileSize = Constants::TILE_SIZE;

    // Calculate visible tile range
    int startTileX = cameraX / tileSize;
    int startTileY = cameraY / tileSize;
    int endTileX = startTileX + Constants::TILES_PER_ROW + 1;
    int endTileY = startTileY + Constants::TILES_PER_COL + 2;  // +2 for partial tile

    // Clamp to map bounds
    startTileX = std::max(0, startTileX);
    startTileY = std::max(0, startTileY);
    endTileX = std::min(width_, endTileX);
    endTileY = std::min(height_, endTileY);

    // Render visible tiles
    for (int y = startTileY; y < endTileY; ++y) {
        for (int x = startTileX; x < endTileX; ++x) {
            const Tile& tile = getTile(x, y);

            // Screen position (offset by camera)
            int screenX = x * tileSize - cameraX;
            int screenY = y * tileSize - cameraY;

            // Source rect from tileset
            SDL_Rect src = tileSet_.getSourceRect(tile.textureX, tile.textureY);
            SDL_Rect dst = {screenX, screenY, tileSize, tileSize};

            renderer.drawTexture(tileSet_.getTexture(), &src, &dst);
        }
    }
}

const Tile& Map::getTile(int x, int y) const {
    if (!isInBounds(x, y)) {
        return defaultTile_;
    }
    return tiles_[y * width_ + x];
}

bool Map::isWalkable(int x, int y) const {
    return isInBounds(x, y) && getTile(x, y).isWalkable();
}

bool Map::isInBounds(int x, int y) const {
    return x >= 0 && x < width_ && y >= 0 && y < height_;
}

void Map::addTransition(const MapTransition& transition) {
    transitions_.push_back(transition);
}

std::optional<MapTransition> Map::getTransitionAt(const Vec2& pos) const {
    for (const auto& transition : transitions_) {
        if (transition.triggerPos == pos) {
            return transition;
        }
    }
    return std::nullopt;
}
