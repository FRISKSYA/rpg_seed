# Dead Code Analysis Report

**Generated:** 2026-01-21
**Project:** RPG Seed
**Baseline Tests:** 551 tests passing

---

## Summary

| Category | Count | Status |
|----------|-------|--------|
| Unused Methods | 2 | SAFE to remove |
| Unused Constants | 2 | SAFE (used indirectly) |
| Unused Includes | 0 | None found |
| Duplicate Code | 1 | CAUTION (refactor candidate) |

---

## Detailed Findings

### SAFE - Unused Methods (Can Remove)

#### 1. `TextRenderer::isLoaded()`
**File:** `src/ui/TextRenderer.h:31`
**Status:** Defined but never called
**Risk:** Low - utility method for debugging
**Action:** Keep for future debugging use

#### 2. `NPCRenderer::isLoaded()`
**File:** `src/entity/NPC.h:52`
**Status:** Defined but never called
**Risk:** Low - utility method for debugging
**Action:** Keep for future debugging use

### SAFE - Constants Used Indirectly

#### 1. `Constants::SCALE_FACTOR`
**File:** `src/util/Constants.h:8`
**Status:** Used to calculate WINDOW_WIDTH and WINDOW_HEIGHT
**Action:** Keep - used in compile-time calculations

#### 2. `Constants::TARGET_FPS`
**File:** `src/util/Constants.h:18`
**Status:** Used to calculate FRAME_DELAY
**Action:** Keep - used in compile-time calculations

### RESOLVED - Duplicate Code Patterns

#### 1. Enemy Name-to-ID Mapping in Game.cpp
**Files:** `src/game/Game.cpp:179-185, 206-211`
**Issue:** Same string comparison chain repeated twice
**Resolution:** Added `findByName()` method to `EnemyDatabase` and refactored Game.cpp to use it
**Status:** ✅ FIXED - Now uses `EnemyDatabase::instance().findByName(name)` instead of hardcoded string comparisons

---

## Methods Verified In Use

| Method | File | Used In |
|--------|------|---------|
| `getTexture()` | ResourceManager.h | Map.cpp |
| `close()` | MenuState.h | test_menu.cpp |
| `close()` | ItemListState.h | test_item_list.cpp |
| `close()` | SaveSlotState.h | test_save_slot.cpp |
| `getCurrentItem()` | MenuState.h | test_menu.cpp |
| `getDefinitionIndex()` | NPC.h | test_npc.cpp |

---

## Cleanup Actions Performed

### Already Cleaned (by previous refactor-cleaner agent)

1. **Removed:** `#include "entity/NPCData.h"` from `src/entity/NPC.cpp`
2. **Added:** `#include <cstring>` to `src/ui/SaveSlotBox.cpp`
3. **Removed:** `#include <SDL_image.h>` from `src/system/Renderer.cpp`

---

## Conclusion

The codebase is well-maintained with minimal dead code. All identified issues have been addressed:
- CRITICAL security issues fixed (dangling reference, path traversal)
- Duplicate enemy name mapping refactored using `EnemyDatabase::findByName()`

**All cleanup tasks complete. 551 tests passing.**
