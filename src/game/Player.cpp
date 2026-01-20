#include "game/Player.h"
#include "field/Map.h"
#include "system/Renderer.h"
#include "system/ResourceManager.h"

Player Player::tryMove(Direction dir, const Map& map) const {
    if (isMoving_ || dir == Direction::None) {
        // Can't start new movement while moving or if no direction
        return Player{tilePos_, targetPos_, dir != Direction::None ? dir : facing_, moveProgress_, isMoving_};
    }

    Vec2 offset = directionToOffset(dir);
    Vec2 newTarget = tilePos_.add(offset);

    // Check if target tile is walkable
    if (map.isWalkable(newTarget.x, newTarget.y)) {
        // Start movement
        return Player{tilePos_, newTarget, dir, 0, true};
    }

    // Can't move - just face the direction
    return Player{tilePos_, tilePos_, dir, 0, false};
}

Player Player::update() const {
    if (!isMoving_) {
        return *this;
    }

    int newProgress = moveProgress_ + 1;

    if (newProgress >= Constants::FRAMES_PER_TILE) {
        // Movement complete - snap to target
        return Player{targetPos_, targetPos_, facing_, 0, false};
    }

    // Continue movement
    return Player{tilePos_, targetPos_, facing_, newProgress, true};
}

Vec2 Player::getPixelPos() const {
    int baseX = tilePos_.x * Constants::TILE_SIZE;
    int baseY = tilePos_.y * Constants::TILE_SIZE;

    if (isMoving_) {
        Vec2 offset = directionToOffset(facing_);
        int pixels = (moveProgress_ * Constants::PLAYER_SPEED);
        baseX += offset.x * pixels;
        baseY += offset.y * pixels;
    }

    return Vec2{baseX, baseY};
}

// PlayerRenderer implementation
PlayerRenderer::PlayerRenderer()
    : texture_(nullptr)
    , spriteWidth_(Constants::TILE_SIZE)
    , spriteHeight_(Constants::TILE_SIZE)
    , frameCounter_(0) {}

bool PlayerRenderer::loadSprite(ResourceManager& resourceManager, const std::string& path) {
    texture_ = resourceManager.loadTexture(path);
    return texture_ != nullptr;
}

void PlayerRenderer::render(Renderer& renderer, const Player& player, int cameraX, int cameraY) {
    if (!texture_) return;

    Vec2 pixelPos = player.getPixelPos();
    int screenX = pixelPos.x - cameraX;
    int screenY = pixelPos.y - cameraY;

    // Update animation frame with wrap-around to prevent overflow
    frameCounter_ = (frameCounter_ + 1) % (Constants::ANIMATION_FRAME_DIVISOR * Constants::WALK_ANIMATION_FRAMES * 1000);
    int frame = 0;
    if (player.isMoving()) {
        frame = (frameCounter_ / Constants::ANIMATION_FRAME_DIVISOR) % Constants::WALK_ANIMATION_FRAMES;
    }

    SDL_Rect src = getSourceRect(player.getFacing(), frame);
    SDL_Rect dst = {screenX, screenY, spriteWidth_, spriteHeight_};

    renderer.drawTexture(texture_, &src, &dst);
}

SDL_Rect PlayerRenderer::getSourceRect(Direction dir, int frame) const {
    // Sprite sheet layout: 4 columns (frames) x 4 rows (directions)
    // Row 0: Down, Row 1: Left, Row 2: Right, Row 3: Up
    int row = 0;
    switch (dir) {
        case Direction::Down:  row = 0; break;
        case Direction::Left:  row = 1; break;
        case Direction::Right: row = 2; break;
        case Direction::Up:    row = 3; break;
        default:               row = 0; break;
    }

    return SDL_Rect{
        frame * spriteWidth_,
        row * spriteHeight_,
        spriteWidth_,
        spriteHeight_
    };
}
