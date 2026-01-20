#include "entity/NPC.h"
#include "entity/NPCData.h"
#include "system/ResourceManager.h"
#include "system/Renderer.h"
#include <cmath>

NPC::NPC(Vec2 pos, Direction facing, int definitionIndex, int spriteRow,
         std::vector<std::string> dialogue)
    : posX_(pos.x)
    , posY_(pos.y)
    , facing_(facing)
    , definitionIndex_(definitionIndex)
    , spriteRow_(spriteRow)
    , dialogue_(std::move(dialogue)) {}

NPC NPC::faceToward(Vec2 targetPos) const {
    int dx = targetPos.x - posX_;
    int dy = targetPos.y - posY_;

    Direction newFacing = facing_;

    // Determine direction based on larger axis difference
    if (std::abs(dx) >= std::abs(dy)) {
        newFacing = (dx > 0) ? Direction::Right : Direction::Left;
    } else {
        newFacing = (dy > 0) ? Direction::Down : Direction::Up;
    }

    return NPC{Vec2{posX_, posY_}, newFacing, definitionIndex_, spriteRow_, dialogue_};
}

// NPCRenderer implementation
NPCRenderer::NPCRenderer()
    : texture_(nullptr)
    , spriteWidth_(Constants::TILE_SIZE)
    , spriteHeight_(Constants::TILE_SIZE)
    , frameCounter_(0) {}

bool NPCRenderer::loadSprites(ResourceManager& resourceManager, const std::string& path) {
    texture_ = resourceManager.loadTexture(path);
    return texture_ != nullptr;
}

void NPCRenderer::render(Renderer& renderer, const NPC& npc, int cameraX, int cameraY) {
    if (!texture_) return;

    Vec2 pixelPos = Vec2{
        npc.getPosition().x * Constants::TILE_SIZE,
        npc.getPosition().y * Constants::TILE_SIZE
    };

    int screenX = pixelPos.x - cameraX;
    int screenY = pixelPos.y - cameraY;

    // Update animation frame with safe wrap-around
    constexpr int MAX_FRAME_COUNT = 60000;  // Safe from overflow, ~16 min at 60fps
    frameCounter_ = (frameCounter_ + 1) % MAX_FRAME_COUNT;
    int frame = (frameCounter_ / (Constants::ANIMATION_FRAME_DIVISOR * 4)) % 2;

    SDL_Rect src = getSourceRect(npc.getFacing(), npc.getSpriteRow(), frame);
    SDL_Rect dst = {screenX, screenY, spriteWidth_, spriteHeight_};

    renderer.drawTexture(texture_, &src, &dst);
}

SDL_Rect NPCRenderer::getSourceRect(Direction dir, int spriteRow, int frame) const {
    // Sprite sheet layout: 4 columns (frames) x N rows (NPC types * 4 directions)
    // Each NPC type has 4 rows for Down, Left, Right, Up
    int dirOffset = 0;
    switch (dir) {
        case Direction::Down:  dirOffset = 0; break;
        case Direction::Left:  dirOffset = 1; break;
        case Direction::Right: dirOffset = 2; break;
        case Direction::Up:    dirOffset = 3; break;
        default:               dirOffset = 0; break;
    }

    // Clamp spriteRow to valid range
    constexpr int MAX_NPC_TYPES = 8;
    if (spriteRow < 0 || spriteRow >= MAX_NPC_TYPES) {
        spriteRow = 0;
    }

    int row = spriteRow * 4 + dirOffset;

    return SDL_Rect{
        frame * spriteWidth_,
        row * spriteHeight_,
        spriteWidth_,
        spriteHeight_
    };
}
