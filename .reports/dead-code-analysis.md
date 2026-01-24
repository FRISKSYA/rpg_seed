# Dead Code Analysis Report

**Generated:** 2026-01-25 (Updated)
**Project:** RPG Seed (C++ SDL2 Game)
**Test Status:** 516 tests passing

---

## Summary

| Severity | Count | Action |
|----------|-------|--------|
| SAFE | 1 | Can remove safely |
| CAUTION | 8 | Used in tests only - keep for planned features |
| DANGER | 0 | None identified |

---

## SAFE: Dead Code Removed

### 1. `EnemyDatabase::findByName()` - REMOVED

**File:** `src/battle/EnemyDatabase.h` (was lines 29-36)
**Status:** Removed on 2026-01-25

Method was never called anywhere in codebase. `findById()` is the standard lookup method.

---

## CAUTION: Test-Only Usage (Keep for Planned Features)

### 1. `language/Word.h` and `language/WordDatabase.h`

**Files:**
- `src/language/Word.h`
- `src/language/WordDatabase.h`

**Status:** Currently unused in production code
**Evidence:**
- Not included in any `.cpp` files
- Created as part of Esperanto learning infrastructure
- Only `TopicDatabase` (conversations) is actively used

**Recommendation:** KEEP - Part of planned vocabulary learning feature

---

### 2. `PlayerStats::withMP()` - Test Only

**File:** `src/game/PlayerStats.h`

**Usages:** tests/test_player_stats.cpp only (7 usages)

**Recommendation:** Keep - MP (mana) system planned feature

---

### 3. `Inventory::removeItem()` - Test Only

**File:** `src/inventory/Inventory.h:57`

**Usages:** tests/test_inventory.cpp only (8 usages)

**Recommendation:** Keep - useful API for future features

---

### 4. Tile Factory Methods (Partial Usage)

**File:** `src/field/Tile.h`

| Method | Production | Tests |
|--------|------------|-------|
| `Tile::grass()` | Yes | Yes |
| `Tile::wall()` | Yes | Yes |
| `Tile::floor()` | No | Yes |
| `Tile::water()` | No | Yes |
| `Tile::tree()` | No | Yes |
| `Tile::mountain()` | No | Yes |
| `Tile::sand()` | No | Yes |
| `Tile::door()` | No | Yes |
| `Tile::bridge()` | No | Yes |
| `Tile::stairs()` | No | Yes |

**Recommendation:** Keep - will be used when more maps are added

---

### 5. `Item::equipment()` and `Item::keyItem()` Factories

**File:** `src/inventory/Item.h`

**Production usages:** ItemDatabase initialization only
**Recommendation:** Keep - planned equipment/quest system

---

### 6. `Item::isEquippable()` - Test Only

**File:** `src/inventory/Item.h:37`

**Recommendation:** Keep - needed for equipment system

---

### 7. `MenuState::close()` and `ItemListState::close()`

**Status:** Never called - code uses `inactive()` directly

**Recommendation:** Consider removing - redundant with `inactive()`

---

## Previously Removed (Verified Clean)

The following dead code was removed in previous cleanup:
- `src/battle/DamageCalculator.h` - Removed
- `tests/test_damage_calc.cpp` - Removed
- `BattleState::selectAttack()` - Removed
- `BattlePhase::EnemyAction` - Removed
- `BattlePhase::Defeat` - Removed

---

## Build Verification

```
$ make clean && make 2>&1 | grep -E "warning:"
(no warnings)
```

Codebase compiles cleanly with `-Wall -Wextra -Wunused`.

---

## Cleanup Summary

| Item | Action | Tests Before | Tests After |
|------|--------|--------------|-------------|
| `EnemyDatabase::findByName()` | Removed | 517 pass | 517 pass |
| Menu tests (5 failures) | Fixed for PhraseBook | 516 pass | 517 pass |

All safe deletions completed successfully.
