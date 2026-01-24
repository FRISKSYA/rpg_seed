# Dead Code Analysis Report

**Generated:** 2026-01-24
**Project:** RPG Seed
**Baseline Tests:** 551 tests passing

---

## Summary

| Category | Count | Status |
|----------|-------|--------|
| Unused Methods | 3 | SAFE - utility/debug methods |
| Test-Only Methods | 8 | CAUTION - keep for testing |
| Future-Use Methods | 4 | KEEP - designed for expansion |
| Duplicate Code | 0 | None found |

---

## Detailed Findings

### SAFE - Unused Utility Methods (Consider Removing)

These methods are defined but never called in production code. They may be useful for debugging.

#### 1. `TextRenderer::isLoaded()`
- **File:** `src/ui/TextRenderer.h:31`
- **Status:** Defined but never called (except in header)
- **Risk:** Low - utility method for debugging
- **Recommendation:** KEEP - useful for future error checking

#### 2. `NPCRenderer::isLoaded()`
- **File:** `src/entity/NPC.h:52`
- **Status:** Defined but never called (except in header)
- **Risk:** Low - utility method for debugging
- **Recommendation:** KEEP - useful for future error checking

#### 3. `TextRenderer::measureText()`
- **File:** `src/ui/TextRenderer.h:28`, `src/ui/TextRenderer.cpp:46-55`
- **Status:** Implemented but never called
- **Risk:** Low - utility for text layout calculations
- **Recommendation:** KEEP - will be useful for centered text, wrapping

---

### CAUTION - Test-Only Methods

These methods are only used in tests. They provide value for testing the API.

| Method | File | Used In |
|--------|------|---------|
| `MenuState::close()` | `MenuState.h:87` | `test_menu.cpp` |
| `ItemListState::close()` | `ItemListState.h:59` | `test_item_list.cpp` |
| `SaveSlotState::close()` | `SaveSlotState.h:57` | `test_save_slot.cpp` |
| `NPC::getDefinitionIndex()` | `NPC.h:26` | `test_npc.cpp` |
| `DialogueState::getCurrentPage()` | `DialogueState.h:52` | `test_dialogue.cpp` |
| `DialogueState::getPageCount()` | `DialogueState.h:53` | `test_dialogue.cpp` |
| `SaveSlotState::getSelectedSlotInfo()` | `SaveSlotState.h:91` | `test_save_slot.cpp` |
| `SaveSlotState::openForLoad()` | `SaveSlotState.h:35` | `test_save_slot.cpp` |

**Action:** KEEP - these methods support the test suite

---

### KEEP - Future-Use Methods (Designed for Expansion)

These methods are part of the public API but not yet used by the game logic.

#### Equipment System (Phase 7+)
- `Item::isEquippable()` - `src/inventory/Item.h:37`
- `Item::equipment()` - `src/inventory/ItemDatabase.h` (factory)
- `Item::keyItem()` - `src/inventory/ItemDatabase.h` (factory)
- `EquipSlot` enum values - `src/inventory/Item.h:14-19`

#### Battle System Extensions
- `DamageCalculator::playerGoesFirst()` - `src/battle/DamageCalculator.h:71`
- `BattleState::getEnemyMaxHP()` - `src/battle/BattleState.h:272`
- `EnemyInstance` class - `src/battle/Enemy.h:69` (not yet used, BattleState stores definition directly)

**Action:** KEEP - these are part of the planned feature roadmap

---

### Constants Used Indirectly (Compile-Time)

These constants are not directly referenced but used in other constant calculations.

| Constant | File | Used By |
|----------|------|---------|
| `SCALE_FACTOR` | `Constants.h:8` | `WINDOW_WIDTH`, `WINDOW_HEIGHT` |
| `TARGET_FPS` | `Constants.h:18` | `FRAME_DELAY` |
| `MIN_TILE_ID` | `Constants.h:30` | (validation range) |

**Action:** KEEP - removing would break compile-time calculations

---

### Tile Factory Methods

All tile factory methods are tested, though some are only used in tests:

| Method | Production Use | Test Use |
|--------|---------------|----------|
| `Tile::grass()` | ✅ Used | ✅ |
| `Tile::water()` | ✅ Used | ✅ |
| `Tile::wall()` | ✅ Used | ✅ |
| `Tile::floor()` | ✅ Used | ✅ |
| `Tile::tree()` | ✅ Used | ✅ |
| `Tile::mountain()` | ❌ | ✅ test_tile.cpp |
| `Tile::sand()` | ❌ | ✅ test_tile.cpp |
| `Tile::bridge()` | ❌ | ✅ test_tile.cpp |
| `Tile::door()` | ❌ | ✅ test_tile.cpp |
| `Tile::stairs()` | ❌ | ✅ test_tile.cpp |

**Action:** KEEP - available for map designers to use

---

### Vec2 Utility Methods

| Method | Production Use | Test Use |
|--------|---------------|----------|
| `add(int, int)` | ✅ Used | ✅ |
| `add(Vec2)` | ✅ Used | ✅ |
| `subtract(Vec2)` | ❌ | ✅ test_vec2.cpp |
| `multiply(int)` | ❌ | ✅ test_vec2.cpp |
| `Vec2::zero()` | ❌ | ✅ test_vec2.cpp |
| `equals(Vec2)` | ✅ Used (via operator==) | ✅ |

**Action:** KEEP - core utility methods, tested and available

---

## Recommendations

### No Action Required

The codebase is well-maintained. All identified "unused" code falls into these categories:

1. **Debugging utilities** - `isLoaded()` methods for checking resource state
2. **Test API** - Methods that make the code testable
3. **Future features** - Equipment system, battle extensions
4. **API completeness** - Vec2 math operations, Tile factories

### Code Quality Notes

- ✅ No orphaned files found
- ✅ No unused includes detected
- ✅ All 551 tests pass
- ✅ Immutability pattern consistently followed
- ✅ Factory methods properly encapsulate construction

---

## Cleanup Actions Performed

**Previous cleanup (2026-01-21):**
1. Removed unused `#include "entity/NPCData.h"` from `src/entity/NPC.cpp`
2. Added missing `#include <cstring>` to `src/ui/SaveSlotBox.cpp`
3. Removed unused `#include <SDL_image.h>` from `src/system/Renderer.cpp`
4. Added `EnemyDatabase::findByName()` to consolidate duplicate lookup code

**No new cleanup needed.**

---

## Conclusion

The codebase has **no actionable dead code**. All unused methods serve valid purposes:
- API completeness for testing
- Debugging utilities
- Future feature support

**All 551 tests passing. No cleanup required.**
