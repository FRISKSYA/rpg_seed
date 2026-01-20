#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <SDL.h>
#include <string>
#include <unordered_map>
#include <memory>

class ResourceManager {
public:
    explicit ResourceManager(SDL_Renderer* renderer);
    ~ResourceManager();

    // Disable copy
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // Texture management
    [[nodiscard]] SDL_Texture* loadTexture(const std::string& path);
    [[nodiscard]] SDL_Texture* getTexture(const std::string& path) const;
    void unloadTexture(const std::string& path);
    void unloadAllTextures();

    // Check if texture exists
    [[nodiscard]] bool hasTexture(const std::string& path) const;

private:
    // Non-owning pointer - renderer must outlive this ResourceManager
    SDL_Renderer* renderer_;

    // Custom deleter for SDL_Texture
    struct TextureDeleter {
        void operator()(SDL_Texture* texture) const {
            if (texture) {
                SDL_DestroyTexture(texture);
            }
        }
    };

    std::unordered_map<std::string, std::unique_ptr<SDL_Texture, TextureDeleter>> textures_;
};

#endif // RESOURCE_MANAGER_H
