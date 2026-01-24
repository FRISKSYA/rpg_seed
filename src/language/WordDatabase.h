#ifndef WORD_DATABASE_H
#define WORD_DATABASE_H

#include "Word.h"
#include <vector>
#include <optional>
#include <unordered_map>

// Singleton database of Esperanto words organized by area level
class WordDatabase {
public:
    // Get singleton instance
    static WordDatabase& instance() {
        static WordDatabase db;
        return db;
    }

    // Find word by Esperanto text
    [[nodiscard]] std::optional<Word> findByEsperanto(const std::string& esperanto) const {
        auto it = esperantoIndex_.find(esperanto);
        if (it != esperantoIndex_.end()) {
            return words_[it->second];
        }
        return std::nullopt;
    }

    // Get all words available at a given area level
    [[nodiscard]] std::vector<Word> getWordsForArea(int areaLevel) const {
        std::vector<Word> result;
        for (const auto& word : words_) {
            if (word.isAvailableAt(areaLevel)) {
                result.push_back(word);
            }
        }
        return result;
    }

    // Get words by category available at area level
    [[nodiscard]] std::vector<Word> getWordsByCategory(
        const std::string& category,
        int areaLevel
    ) const {
        std::vector<Word> result;
        for (const auto& word : words_) {
            if (word.category == category && word.isAvailableAt(areaLevel)) {
                result.push_back(word);
            }
        }
        return result;
    }

    // Get all words
    [[nodiscard]] const std::vector<Word>& getAllWords() const {
        return words_;
    }

    // Prevent copying
    WordDatabase(const WordDatabase&) = delete;
    WordDatabase& operator=(const WordDatabase&) = delete;

private:
    std::vector<Word> words_;
    std::unordered_map<std::string, size_t> esperantoIndex_;

    // Private constructor - initializes all words
    WordDatabase() {
        initializeWords();
    }

    void initializeWords() {
        // Area 1: Basic greetings and responses
        addWord(Word::create("saluton", "こんにちは", 1, "greeting"));
        addWord(Word::create("dankon", "ありがとう", 1, "response"));
        addWord(Word::create("jes", "はい", 1, "response"));
        addWord(Word::create("ne", "いいえ", 1, "response"));
        addWord(Word::create("bonvolu", "お願いします", 1, "request"));
        addWord(Word::create("adiau", "さようなら", 1, "greeting"));
        addWord(Word::create("pardonu", "すみません", 1, "apology"));
        addWord(Word::create("bone", "元気です/良い", 1, "response"));

        // Area 2: Questions and common phrases
        addWord(Word::create("kiel", "どのように", 2, "question"));
        addWord(Word::create("kio", "何", 2, "question"));
        addWord(Word::create("kiu", "誰", 2, "question"));
        addWord(Word::create("kie", "どこ", 2, "question"));
        addWord(Word::create("kiam", "いつ", 2, "question"));
        addWord(Word::create("mi", "私", 2, "pronoun"));
        addWord(Word::create("vi", "あなた", 2, "pronoun"));
        addWord(Word::create("estas", "です/いる", 2, "verb"));

        // Area 3: More complex expressions
        addWord(Word::create("komprenas", "わかる/理解する", 3, "verb"));
        addWord(Word::create("parolas", "話す", 3, "verb"));
        addWord(Word::create("helpas", "助ける", 3, "verb"));
        addWord(Word::create("amiko", "友達", 3, "noun"));
        addWord(Word::create("paco", "平和", 3, "noun"));
        addWord(Word::create("bela", "美しい", 3, "adjective"));
    }

    void addWord(Word word) {
        esperantoIndex_[word.esperanto] = words_.size();
        words_.push_back(std::move(word));
    }
};

#endif // WORD_DATABASE_H
