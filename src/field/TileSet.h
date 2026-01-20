#ifndef TILESET_H
#define TILESET_H

#include <SDL.h>
#include <string>
#include "util/Constants.h"

class ResourceManager;

// Manages a tileset texture for rendering map tiles
class TileSet {
public:
    explicit TileSet(int tileSize = Constants::TILE_SIZE);

    // Load tileset from file
    [[nodiscard]] bool load(ResourceManager& resourceManager, const std::string& path);

    // Get the texture for rendering
    [[nodiscard]] SDL_Texture* getTexture() const { return texture_; }

    // Get source rect for a tile at grid position
    [[nodiscard]] SDL_Rect getSourceRect(int tileX, int tileY) const;

private:
    // Non-owning pointer - texture lifetime managed by ResourceManager
    SDL_Texture* texture_;
    int tileSize_;
};

#endif // TILESET_H
