#include "field/TileSet.h"
#include "system/ResourceManager.h"

TileSet::TileSet(int tileSize) : texture_(nullptr), tileSize_(tileSize) {}

bool TileSet::load(ResourceManager& resourceManager, const std::string& path) {
    texture_ = resourceManager.loadTexture(path);
    if (texture_) {
        path_ = path;
        return true;
    }
    return false;
}

SDL_Rect TileSet::getSourceRect(int tileX, int tileY) const {
    return SDL_Rect{
        tileX * tileSize_,
        tileY * tileSize_,
        tileSize_,
        tileSize_
    };
}
