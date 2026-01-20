#include "system/Input.h"
#include <cstring>

Input::Input() : currentKeyState_(nullptr), quitRequested_(false) {
    std::memset(previousKeyState_, 0, sizeof(previousKeyState_));
    currentKeyState_ = SDL_GetKeyboardState(nullptr);
}

void Input::update() {
    // Store previous state before processing events
    std::memcpy(previousKeyState_, currentKeyState_, SDL_NUM_SCANCODES);

    // Process SDL events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                quitRequested_ = true;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    quitRequested_ = true;
                }
                break;
        }
    }

    // Update keyboard state pointer
    currentKeyState_ = SDL_GetKeyboardState(nullptr);
}

Direction Input::getMovementDirection() const {
    // Priority: Up > Down > Left > Right
    if (currentKeyState_[SDL_SCANCODE_UP] || currentKeyState_[SDL_SCANCODE_W]) {
        return Direction::Up;
    }
    if (currentKeyState_[SDL_SCANCODE_DOWN] || currentKeyState_[SDL_SCANCODE_S]) {
        return Direction::Down;
    }
    if (currentKeyState_[SDL_SCANCODE_LEFT] || currentKeyState_[SDL_SCANCODE_A]) {
        return Direction::Left;
    }
    if (currentKeyState_[SDL_SCANCODE_RIGHT] || currentKeyState_[SDL_SCANCODE_D]) {
        return Direction::Right;
    }
    return Direction::None;
}

bool Input::isKeyPressed(SDL_Scancode key) const {
    return currentKeyState_[key] != 0;
}

bool Input::isKeyJustPressed(SDL_Scancode key) const {
    return currentKeyState_[key] && !previousKeyState_[key];
}

bool Input::isKeyJustReleased(SDL_Scancode key) const {
    return !currentKeyState_[key] && previousKeyState_[key];
}

bool Input::isConfirmPressed() const {
    return isKeyJustPressed(SDL_SCANCODE_Z) || isKeyJustPressed(SDL_SCANCODE_RETURN);
}

bool Input::isCancelPressed() const {
    return isKeyJustPressed(SDL_SCANCODE_X) || isKeyJustPressed(SDL_SCANCODE_BACKSPACE);
}

bool Input::isMenuPressed() const {
    return isKeyJustPressed(SDL_SCANCODE_SPACE) || isKeyJustPressed(SDL_SCANCODE_M);
}
