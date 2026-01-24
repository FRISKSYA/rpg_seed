# ストーリー構造 & NPC管理システム企画書

**作成日:** 2026-01-25
**ステータス:** 設計中
**関連:** [PHRASE_BATTLE_DESIGN.md](./PHRASE_BATTLE_DESIGN.md)

---

## 概要

ゲームのストーリーを「メイン + 複数のサブストーリー」の複合構造として設計し、
NPCの会話・状態をストーリー単位で管理するアーキテクチャ。

**目的:**
- NPCが文脈に応じた自然な会話をする
- ネタバレや矛盾を防ぐ
- 拡張・並行開発がしやすい構造

---

## ストーリー構造

### 全体像

```
┌─────────────────────────────────────────────────────────────┐
│                        ゲーム世界                           │
│                                                             │
│  ┌───────────────────────────────────────────────────────┐ │
│  │                  メインストーリー                       │ │
│  │                                                         │ │
│  │   [章1] ──→ [章2] ──→ [章3] ──→ [章4] ──→ ...        │ │
│  │   村を出る   初めての   ゴブリン   王都へ               │ │
│  │             和解       集落                             │ │
│  └───────────────────────────────────────────────────────┘ │
│         │          │          │          │                 │
│         ▼          ▼          ▼          ▼                 │
│  ┌──────────┐┌──────────┐┌──────────┐┌──────────┐         │
│  │サブ      ││サブ      ││サブ      ││サブ      │         │
│  │ストーリー ││ストーリー ││ストーリー ││ストーリー │         │
│  │群 A      ││群 B      ││群 C      ││群 D      │         │
│  └──────────┘└──────────┘└──────────┘└──────────┘         │
│                                                             │
│  ※ 各章の進行で新しいサブストーリーが解禁される             │
└─────────────────────────────────────────────────────────────┘
```

### メインストーリー

ゲームの背骨となる一本のストーリーライン。

```
章1: 旅立ち
  └─ 主人公が村を出る
  └─ 基本フレーズを習得
  └─ 「魔物は敵」という認識でスタート

章2: 最初の気づき
  └─ 魔物と初めて会話が成立
  └─ 「言葉が通じる？」という驚き
  └─ 会話システムのチュートリアル

章3: 深まる理解
  └─ ゴブリン集落で複雑な事情を知る
  └─ キーワード解錠システム導入
  └─ 「敵ではないのかも」という認識変化

章4-N: （後続展開）
  └─ ...
```

### サブストーリー

メインと並行して進む小〜中規模のストーリー。

**特徴:**
- 特定のNPC・場所に紐づく
- メイン進行度に応じて解禁
- 完了すると報酬（フレーズ、アイテム、情報）

---

## ストーリーユニット設計

### データ構造

```cpp
// ストーリーの状態
enum class StoryState {
    Locked,      // 未解禁（存在を知らない）
    Available,   // 解禁済み（開始可能）
    Active,      // 進行中
    Completed    // 完了
};

// ストーリーユニット
struct StoryUnit {
    // 識別
    std::string id;           // "sub_forest_guardian"
    std::string title;        // "森の番人の孤独"
    StoryType type;           // Main / Sub

    // 解禁条件
    struct UnlockCondition {
        int requiredChapter;                    // メイン章の進行度
        std::vector<std::string> requiredStories;  // 必要な完了ストーリー
        std::optional<std::string> requiredArea;   // 必要なエリア到達
        std::optional<std::string> requiredNpcMet; // 必要なNPC遭遇
    };
    UnlockCondition unlockCondition;

    // 状態
    StoryState state;
    int progress;             // 0-100

    // 関連要素
    std::vector<std::string> involvedNpcIds;   // 関連NPC
    std::vector<std::string> involvedAreaIds;  // 関連エリア

    // 報酬
    struct Rewards {
        std::vector<std::string> phrases;      // 解禁フレーズ
        std::vector<std::string> items;        // 獲得アイテム
        std::vector<std::string> unlockStories; // 解禁される次のストーリー
    };
    Rewards rewards;
};

enum class StoryType {
    Main,        // メインストーリー
    Sub,         // サブストーリー
    Hidden       // 隠しストーリー（特殊条件）
};
```

### ストーリー進行の管理

```cpp
class StoryManager {
public:
    // クエリ
    [[nodiscard]] StoryState getState(const std::string& storyId) const;
    [[nodiscard]] int getProgress(const std::string& storyId) const;
    [[nodiscard]] std::vector<StoryUnit> getAvailableStories() const;
    [[nodiscard]] std::vector<StoryUnit> getActiveStories() const;

    // 状態更新（不変、新しいManagerを返す）
    [[nodiscard]] StoryManager checkUnlocks(const GameContext& context) const;
    [[nodiscard]] StoryManager startStory(const std::string& storyId) const;
    [[nodiscard]] StoryManager advanceProgress(const std::string& storyId, int delta) const;
    [[nodiscard]] StoryManager completeStory(const std::string& storyId) const;

private:
    std::vector<StoryUnit> stories_;
    int currentMainChapter_;
};
```

---

## NPC管理システム

### NPCとストーリーの関係

```
┌─────────────────────────────────────────────────────────────┐
│                        NPC定義                              │
│                                                             │
│  ┌────────────────────────────────────────────────────┐    │
│  │ NPC: 鍛冶屋のガロン                                 │    │
│  │                                                      │    │
│  │ 基本情報:                                            │    │
│  │   - 性格: 無口、職人気質、面倒見がいい              │    │
│  │   - 口調: ぶっきらぼう、短文                        │    │
│  │   - 背景: 元冒険者、過去に仲間を失った              │    │
│  │                                                      │    │
│  │ 関連ストーリー:                                      │    │
│  │   - [常時] メインストーリー                         │    │
│  │   - [章2〜] サブ「鍛冶屋の過去」                    │    │
│  │   - [章4〜] サブ「伝説の剣」                        │    │
│  │                                                      │    │
│  │ 会話コンテキスト:                                    │    │
│  │   → 関連ストーリーの状態を参照して会話を選択       │    │
│  └────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────┘
```

### NPCデータ構造

```cpp
struct NpcDefinition {
    // 識別
    std::string id;
    std::string name;

    // 基本設定
    struct Personality {
        std::string trait;        // "無口、職人気質"
        std::string speechStyle;  // "ぶっきらぼう、短文"
        std::string background;   // 背景設定（内部参照用）
    };
    Personality personality;

    // ストーリー関連
    std::vector<std::string> relatedStoryIds;

    // 会話ルール（後述）
    std::vector<DialogueRule> dialogueRules;
};
```

### 会話選択ロジック

NPCの会話は「ルールベース」で選択される。

```cpp
struct DialogueRule {
    // このルールが適用される条件
    struct Condition {
        std::optional<std::string> storyId;      // 特定ストーリーの状態
        std::optional<StoryState> storyState;    // Active, Completed, etc.
        std::optional<int> minProgress;          // 最小進行度
        std::optional<int> maxProgress;          // 最大進行度
        std::optional<int> minAffinity;          // 最小好感度
        std::optional<std::string> hasPhrase;    // 特定フレーズ所持
        std::optional<std::string> timeOfDay;    // 時間帯
    };
    Condition condition;

    // 条件を満たした時の会話
    std::vector<std::string> dialoguePool;  // ランダムに選ばれる

    // 優先度（高いほど優先）
    int priority;
};
```

### 会話選択フロー

```
プレイヤーがNPCに話しかける
        │
        ▼
┌─────────────────────────────────────┐
│ 現在のコンテキストを収集            │
│ - メイン章: 3                       │
│ - アクティブストーリー: [...]       │
│ - 好感度: 45                        │
│ - 所持フレーズ: [...]               │
│ - 時間帯: 夜                        │
└─────────────────────────────────────┘
        │
        ▼
┌─────────────────────────────────────┐
│ NPCの全DialogueRuleを評価           │
│                                     │
│ Rule 1: storyId="past" && Active    │
│         && progress >= 50           │
│         → 優先度: 100               │
│         → 条件: ✓ 満たす            │
│                                     │
│ Rule 2: affinity >= 30              │
│         → 優先度: 50                │
│         → 条件: ✓ 満たす            │
│                                     │
│ Rule 3: (デフォルト)                │
│         → 優先度: 0                 │
│         → 条件: ✓ 常に満たす        │
└─────────────────────────────────────┘
        │
        ▼
┌─────────────────────────────────────┐
│ 最も優先度の高いルールを選択        │
│ → Rule 1 (優先度100)                │
│                                     │
│ dialoguePoolからランダムに1つ選択   │
│ → 「あの時のこと...話す気になった」 │
└─────────────────────────────────────┘
```

---

## 具体例: 森の番人ストーリー

### ストーリー定義

```yaml
id: sub_forest_guardian
title: 森の番人の孤独
type: Sub

unlockCondition:
  requiredChapter: 2
  requiredArea: forest_entrance

involvedNpcs:
  - forest_guardian
  - village_elder    # ヒントをくれる

rewards:
  phrases:
    - "ni_estas_amikoj"  # 「私たちは友達だ」
  unlockStories:
    - sub_ancient_ruins  # 次のストーリー解禁
```

### 進行フロー

```
┌─────────────────────────────────────────────────────────────┐
│ Progress 0%: 解禁直後                                       │
│                                                             │
│ [森の番人に話しかける]                                      │
│ 番人「......」                                              │
│ 番人「通さん。帰れ」                                        │
│                                                             │
│ → どのフレーズを使っても会話拒否                           │
│ → ヒント: 村で情報を集める必要がある                       │
└─────────────────────────────────────────────────────────────┘
        │
        ▼
┌─────────────────────────────────────────────────────────────┐
│ Progress 30%: 村長から情報を得る                            │
│                                                             │
│ [村長に話しかける]                                          │
│ 村長「森の番人か...あいつは昔、仲間を失ってな」            │
│ 村長「それ以来、誰も信じなくなった」                        │
│ 村長「『理解している』と伝えれば、心を開くかもしれん」      │
│                                                             │
│ → フレーズヒント取得: 「共感」系が有効                     │
│ → 具体的なフレーズは教えてもらえない（自分で探す）         │
└─────────────────────────────────────────────────────────────┘
        │
        ▼
┌─────────────────────────────────────────────────────────────┐
│ Progress 50%: 心を開く                                      │
│                                                             │
│ [森の番人に「Mi komprenas vin」を使う]                      │
│ （私はあなたを理解しています）                              │
│                                                             │
│ 番人「......」                                              │
│ 番人「お前...なぜその言葉を知っている」                     │
│ 番人「...少しだけ話を聞いてやる」                          │
│                                                             │
│ → 会話解錠、親密度上昇                                     │
└─────────────────────────────────────────────────────────────┘
        │
        ▼
┌─────────────────────────────────────────────────────────────┐
│ Progress 70%: 過去を聞く                                    │
│                                                             │
│ [番人と会話を重ねる]                                        │
│ 番人「昔、俺にも仲間がいた」                                │
│ 番人「だが、人間に裏切られた」                              │
│ 番人「それ以来、俺は一人でこの森を守っている」              │
│                                                             │
│ → ストーリー背景が明かされる                               │
└─────────────────────────────────────────────────────────────┘
        │
        ▼
┌─────────────────────────────────────────────────────────────┐
│ Progress 100%: 友人になる                                   │
│                                                             │
│ [親密度MAXに到達]                                           │
│ 番人「お前は...信じてもいいのかもしれん」                   │
│ 番人「これを教えてやる。『Ni estas amikoj』」              │
│ 番人「『私たちは友達だ』という意味だ。大切に使え」          │
│                                                             │
│ → フレーズ「Ni estas amikoj」習得！                        │
│ → ストーリー完了                                           │
│ → 古代遺跡ストーリー解禁                                   │
└─────────────────────────────────────────────────────────────┘
```

### NPC会話ルール

```yaml
npc: forest_guardian
dialogueRules:
  # ストーリー完了後
  - condition:
      storyId: sub_forest_guardian
      storyState: Completed
    priority: 100
    dialoguePool:
      - "...元気にしているか"
      - "森は今日も平和だ"
      - "お前が来ると、少し気が楽になる"

  # ストーリー進行中（心を開いた後）
  - condition:
      storyId: sub_forest_guardian
      storyState: Active
      minProgress: 50
    priority: 90
    dialoguePool:
      - "...何か聞きたいことがあるのか"
      - "まあ、座れ。話を聞いてやる"

  # ストーリー進行中（心を開く前）
  - condition:
      storyId: sub_forest_guardian
      storyState: Active
      maxProgress: 49
    priority: 80
    dialoguePool:
      - "......通さん"
      - "帰れ"
      - "人間と話すことはない"

  # デフォルト（ストーリー未解禁）
  - condition: {}
    priority: 0
    dialoguePool:
      - "......」
      - "（こちらを睨んでいる）"
```

---

## セーブデータとの統合

### 保存が必要なデータ

```cpp
struct StorySaveData {
    // メイン進行度
    int mainChapter;

    // 各ストーリーの状態
    struct StoryProgress {
        std::string storyId;
        StoryState state;
        int progress;
    };
    std::vector<StoryProgress> storyProgresses;

    // NPC好感度
    std::map<std::string, int> npcAffinities;

    // 会話フラグ（特定の会話を見たか）
    std::set<std::string> seenDialogueIds;
};
```

---

## 開発ワークフロー

### ストーリー追加手順

```
1. ストーリー企画
   └─ 概要、関連NPC、報酬を決める

2. ストーリーユニット定義
   └─ YAML/JSONでデータ作成

3. NPC会話ルール追加
   └─ 各進行度での会話を定義

4. テスト
   └─ 解禁条件、進行、報酬の確認

5. マージ
   └─ 他ストーリーとの整合性確認
```

### ファイル構成案

```
data/
├── stories/
│   ├── main/
│   │   ├── chapter1.yaml
│   │   ├── chapter2.yaml
│   │   └── ...
│   └── sub/
│       ├── forest_guardian.yaml
│       ├── blacksmith_past.yaml
│       └── ...
├── npcs/
│   ├── forest_guardian.yaml
│   ├── blacksmith.yaml
│   ├── village_elder.yaml
│   └── ...
└── dialogues/
    ├── forest_guardian_dialogues.yaml
    ├── blacksmith_dialogues.yaml
    └── ...
```

---

## 今後の検討事項

### 優先度高

- [ ] メインストーリーの章立て詳細
- [ ] 最初のサブストーリー3〜5個の設計
- [ ] NPC会話データのフォーマット確定

### 優先度中

- [ ] 時間帯・天候による会話変化
- [ ] NPC同士の関係性（AがBについて話す）
- [ ] 繰り返し会話の多様性確保

### 優先度低

- [ ] 隠しストーリーの設計
- [ ] マルチエンディング対応
- [ ] NPC会話のローカライズ対応

---

## 参考

- **Dwarf Fortress**: 履歴生成によるNPCの深み
- **Disco Elysium**: スキルベースの会話分岐
- **Undertale**: キャラクター記憶と一貫性

---

## 変更履歴

| 日付 | 内容 |
|------|------|
| 2026-01-25 | 初版作成 |
