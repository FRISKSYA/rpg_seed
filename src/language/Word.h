#ifndef WORD_H
#define WORD_H

#include <string>

// Immutable Esperanto word definition
// Maps Esperanto word to Japanese translation with area level
struct Word {
    const std::string esperanto;   // Esperanto word (e.g., "saluton")
    const std::string japanese;    // Japanese translation (e.g., "こんにちは")
    const int areaLevel;           // Area level where this word is introduced (1-based)
    const std::string category;    // Category (greeting, response, question, etc.)

    // Factory method to create word
    static Word create(
        std::string esperanto,
        std::string japanese,
        int areaLevel,
        std::string category = "general"
    ) {
        return Word{
            std::move(esperanto),
            std::move(japanese),
            areaLevel,
            std::move(category)
        };
    }

    // Check if word is available at given area level
    [[nodiscard]] bool isAvailableAt(int currentAreaLevel) const {
        return areaLevel <= currentAreaLevel;
    }

private:
    // Private constructor (use factory method)
    Word(
        std::string esperanto,
        std::string japanese,
        int areaLevel,
        std::string category
    ) : esperanto(std::move(esperanto)),
        japanese(std::move(japanese)),
        areaLevel(areaLevel),
        category(std::move(category)) {}
};

#endif // WORD_H
