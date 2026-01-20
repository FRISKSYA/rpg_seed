#ifndef NPC_H
#define NPC_H

#include <SDL.h>
#include <string>
#include <vector>
#include "util/Vec2.h"
#include "util/Constants.h"

class ResourceManager;
class Renderer;
struct NPCDefinition;

// Immutable NPC entity
class NPC {
public:
    // definitionIndex references into the Map's npcDefinitions_ vector (safer than pointer)
    NPC(Vec2 pos, Direction facing, int definitionIndex, int spriteRow,
        std::vector<std::string> dialogue);

    // Getters
    [[nodiscard]] Vec2 getPosition() const { return Vec2{posX_, posY_}; }
    [[nodiscard]] Direction getFacing() const { return facing_; }
    [[nodiscard]] int getSpriteRow() const { return spriteRow_; }
    [[nodiscard]] const std::vector<std::string>& getDialogue() const { return dialogue_; }
    [[nodiscard]] int getDefinitionIndex() const { return definitionIndex_; }

    // Face toward a position (returns new NPC)
    [[nodiscard]] NPC faceToward(Vec2 targetPos) const;

private:
    int posX_;
    int posY_;
    Direction facing_;
    int definitionIndex_;
    int spriteRow_;
    std::vector<std::string> dialogue_;  // Copy of dialogue, no pointer needed
};

// Renders NPC sprites
class NPCRenderer {
public:
    NPCRenderer();

    // Load NPC sprite sheet
    [[nodiscard]] bool loadSprites(ResourceManager& resourceManager, const std::string& path);

    // Render an NPC
    void render(Renderer& renderer, const NPC& npc, int cameraX, int cameraY);

    // Check if sprites are loaded
    [[nodiscard]] bool isLoaded() const { return texture_ != nullptr; }

private:
    // Get source rect for NPC sprite
    [[nodiscard]] SDL_Rect getSourceRect(Direction dir, int spriteRow, int frame) const;

    // Non-owning pointer - texture lifetime managed by ResourceManager
    SDL_Texture* texture_;
    int spriteWidth_;
    int spriteHeight_;
    int frameCounter_;
};

#endif // NPC_H
