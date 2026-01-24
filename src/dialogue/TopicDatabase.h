#ifndef TOPIC_DATABASE_H
#define TOPIC_DATABASE_H

#include "ConversationTopic.h"
#include <vector>
#include <optional>
#include <unordered_map>
#include <random>
#include <ctime>

// Singleton database of conversation topics organized by area level
class TopicDatabase {
public:
    // Get singleton instance
    static TopicDatabase& instance() {
        static TopicDatabase db;
        return db;
    }

    // Find topic by ID
    [[nodiscard]] std::optional<ConversationTopic> findById(const std::string& id) const {
        auto it = topicIndex_.find(id);
        if (it != topicIndex_.end()) {
            return topics_[it->second];
        }
        return std::nullopt;
    }

    // Get all topics available at a given area level
    [[nodiscard]] std::vector<ConversationTopic> getTopicsForArea(int areaLevel) const {
        std::vector<ConversationTopic> result;
        for (const auto& topic : topics_) {
            if (topic.areaLevel <= areaLevel) {
                result.push_back(topic);
            }
        }
        return result;
    }

    // Get a random topic for the given area level
    [[nodiscard]] std::optional<ConversationTopic> getRandomTopicForArea(int areaLevel) const {
        auto available = getTopicsForArea(areaLevel);
        if (available.empty()) {
            return std::nullopt;
        }

        static std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));
        std::uniform_int_distribution<size_t> dist(0, available.size() - 1);
        return available[dist(rng)];
    }

    // Get all topics
    [[nodiscard]] const std::vector<ConversationTopic>& getAllTopics() const {
        return topics_;
    }

    // Prevent copying
    TopicDatabase(const TopicDatabase&) = delete;
    TopicDatabase& operator=(const TopicDatabase&) = delete;

private:
    std::vector<ConversationTopic> topics_;
    std::unordered_map<std::string, size_t> topicIndex_;

    // Private constructor - initializes all topics
    TopicDatabase() {
        initializeTopics();
    }

    void initializeTopics() {
        // Area 1: Basic greeting conversations
        addTopic(ConversationTopic::create(
            "greeting_basic",
            "Saluton!",
            "こんにちは！",
            {
                ConversationChoice::create("Saluton!", "こんにちは！", true, 25),
                ConversationChoice::create("Dankon!", "ありがとう！", false, 5),
                ConversationChoice::create("...", "（無言）", false, -5)
            },
            1
        ));

        addTopic(ConversationTopic::create(
            "thanks_response",
            "Dankon pro via helpo!",
            "手伝ってくれてありがとう！",
            {
                ConversationChoice::create("Ne dankinde!", "どういたしまして！", true, 30),
                ConversationChoice::create("Jes!", "はい！", false, 5),
                ConversationChoice::create("...", "（無言）", false, -5)
            },
            1
        ));

        addTopic(ConversationTopic::create(
            "how_are_you",
            "Kiel vi fartas?",
            "お元気ですか？",
            {
                ConversationChoice::create("Bone, dankon!", "元気です、ありがとう！", true, 25),
                ConversationChoice::create("Saluton!", "こんにちは！", false, 0),
                ConversationChoice::create("...", "（無言）", false, -5)
            },
            1
        ));

        addTopic(ConversationTopic::create(
            "farewell",
            "Mi devas iri nun. Adiau!",
            "もう行かなくては。さようなら！",
            {
                ConversationChoice::create("Adiau! Gis revido!", "さようなら！また会いましょう！", true, 30),
                ConversationChoice::create("Jes!", "はい！", false, 5),
                ConversationChoice::create("...", "（無言）", false, -5)
            },
            1
        ));

        // Area 2: Question-based conversations
        addTopic(ConversationTopic::create(
            "who_are_you",
            "Kiu vi estas?",
            "あなたは誰ですか？",
            {
                ConversationChoice::create("Mi estas aventuristo.", "私は冒険者です。", true, 25),
                ConversationChoice::create("Saluton!", "こんにちは！", false, 0),
                ConversationChoice::create("...", "（無言）", false, -10)
            },
            2
        ));

        addTopic(ConversationTopic::create(
            "where_from",
            "De kie vi venas?",
            "どこから来ましたか？",
            {
                ConversationChoice::create("Mi venas de malproksime.", "遠くから来ました。", true, 25),
                ConversationChoice::create("Mi ne komprenas.", "わかりません。", false, 5),
                ConversationChoice::create("...", "（無言）", false, -10)
            },
            2
        ));

        // Area 3: More complex conversations
        addTopic(ConversationTopic::create(
            "need_help",
            "Cu vi povas helpi min?",
            "手伝ってくれますか？",
            {
                ConversationChoice::create("Jes, mi volonte helpas!", "はい、喜んで手伝います！", true, 35),
                ConversationChoice::create("Pardonu, mi ne povas.", "すみません、できません。", false, -5),
                ConversationChoice::create("...", "（無言）", false, -15)
            },
            3
        ));

        addTopic(ConversationTopic::create(
            "be_friends",
            "Cu vi volas esti mia amiko?",
            "友達になりませんか？",
            {
                ConversationChoice::create("Jes! Ni estos amikoj!", "はい！友達になりましょう！", true, 40),
                ConversationChoice::create("Mi pensas pri tio.", "考えてみます。", false, 10),
                ConversationChoice::create("...", "（無言）", false, -10)
            },
            3
        ));
    }

    void addTopic(ConversationTopic topic) {
        topicIndex_[topic.id] = topics_.size();
        topics_.push_back(std::move(topic));
    }
};

#endif // TOPIC_DATABASE_H
