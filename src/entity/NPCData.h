#ifndef NPC_DATA_H
#define NPC_DATA_H

#include <string>
#include <vector>
#include "util/Vec2.h"

// NPC definition (shared data for NPC type)
struct NPCDefinition {
    std::string id;
    int spriteRow;  // Row in NPC sprite sheet
    std::vector<std::string> dialogue;  // Dialogue pages

    NPCDefinition() : id(""), spriteRow(0), dialogue{} {}
    NPCDefinition(std::string i, int row, std::vector<std::string> d)
        : id(std::move(i)), spriteRow(row), dialogue(std::move(d)) {}
};

#endif // NPC_DATA_H
