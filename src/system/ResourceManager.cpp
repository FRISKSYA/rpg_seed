#include "system/ResourceManager.h"
#include <SDL_image.h>
#include <iostream>

ResourceManager::ResourceManager(SDL_Renderer* renderer) : renderer_(renderer) {}

ResourceManager::~ResourceManager() {
    unloadAllTextures();
}

SDL_Texture* ResourceManager::loadTexture(const std::string& path) {
    // Security: Validate path to prevent directory traversal attacks
    if (path.find("..") != std::string::npos ||
        path.find('/') == 0 ||
        path.find('\\') != std::string::npos) {
        std::cerr << "Invalid texture path: path traversal not allowed" << std::endl;
        return nullptr;
    }

    // Check if already loaded
    auto it = textures_.find(path);
    if (it != textures_.end()) {
        return it->second.get();
    }

    // Load new texture
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "Failed to load image - " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        std::cerr << "Failed to create texture - " << SDL_GetError() << std::endl;
        return nullptr;
    }

    textures_[path] = std::unique_ptr<SDL_Texture, TextureDeleter>(texture);
    return texture;
}

SDL_Texture* ResourceManager::getTexture(const std::string& path) const {
    auto it = textures_.find(path);
    if (it != textures_.end()) {
        return it->second.get();
    }
    return nullptr;
}

void ResourceManager::unloadTexture(const std::string& path) {
    textures_.erase(path);
}

void ResourceManager::unloadAllTextures() {
    textures_.clear();
}

bool ResourceManager::hasTexture(const std::string& path) const {
    return textures_.find(path) != textures_.end();
}
