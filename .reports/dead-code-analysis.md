# Dead Code Analysis Report

Generated: 2026-01-25

## Summary

| Category | Count | Action |
|----------|-------|--------|
| SAFE to delete | 1 | Ready for removal |
| CAUTION | 1 | Keep for now |
| DANGER | 0 | Do not touch |

---

## SAFE: Dead Code - Ready for Removal

### 1. `src/battle/DamageCalculator.h`

**Status:** DEAD CODE - No longer used in production

**Evidence:**
- Not included in any production source files
- Only referenced in `tests/test_damage_calc.cpp`
- The new affinity-based battle system does not use HP damage calculations
- Attack command was removed from BattleState

**Impact:** Low - utility class with no side effects

**Recommendation:** Delete both:
- `src/battle/DamageCalculator.h`
- `tests/test_damage_calc.cpp`

---

## CAUTION: Unused but Potentially Useful

### 1. `src/language/WordDatabase.h` and `src/language/Word.h`

**Status:** Currently unused in production code

**Evidence:**
- Not included in any `.cpp` files
- Created as part of Esperanto learning infrastructure
- Only `TopicDatabase` (conversations) is actively used

**Impact:** Low - these are part of planned vocabulary learning feature

**Recommendation:** KEEP - Part of Phase 5 content infrastructure. May be used for:
- Vocabulary study mode
- Word glossary feature
- Progress tracking per word

---

## Analysis Details

### Files Checked
- All `src/**/*.h` and `src/**/*.cpp` files
- All `tests/*.cpp` files

### Methods Verified as Removed
The following old battle methods were successfully removed:
- `BattleState::selectAttack()` - Removed
- `BattleState::executeEnemyAction()` - Removed
- `BattleState::getEnemyHp()` - Removed
- `BattlePhase::EnemyAction` - Removed
- `BattlePhase::Defeat` - Removed

### New Code Properly Integrated
- `ConversationTopic.h` - Used in BattleState.h
- `TopicDatabase.h` - Used in Game.cpp
- `Personality` enum - Used in BattleState.h, Game.cpp
- Affinity system - Fully integrated

---

## Proposed Deletions

\`\`\`
DELETE: src/battle/DamageCalculator.h
DELETE: tests/test_damage_calc.cpp
\`\`\`

**Test verification required before deletion.**
