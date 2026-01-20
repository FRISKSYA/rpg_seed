#ifndef INPUT_H
#define INPUT_H

#include <SDL.h>
#include "util/Vec2.h"

class Input {
public:
    Input();

    // Update input state (call once per frame)
    void update();

    // Check if quit was requested
    [[nodiscard]] bool isQuitRequested() const { return quitRequested_; }

    // Get movement direction based on current key state
    [[nodiscard]] Direction getMovementDirection() const;

    // Key state queries
    [[nodiscard]] bool isKeyPressed(SDL_Scancode key) const;
    [[nodiscard]] bool isKeyJustPressed(SDL_Scancode key) const;

    // Action queries
    [[nodiscard]] bool isConfirmPressed() const;
    [[nodiscard]] bool isCancelPressed() const;
    [[nodiscard]] bool isMenuPressed() const;

    // Menu navigation (just-pressed for single-step navigation)
    [[nodiscard]] bool isMenuUpPressed() const;
    [[nodiscard]] bool isMenuDownPressed() const;

private:
    const Uint8* currentKeyState_;
    Uint8 previousKeyState_[SDL_NUM_SCANCODES];
    bool quitRequested_;
};

#endif // INPUT_H
