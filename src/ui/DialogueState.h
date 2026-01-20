#ifndef DIALOGUE_STATE_H
#define DIALOGUE_STATE_H

#include <string>
#include <vector>

// Dialogue page (text content)
struct DialoguePage {
    std::string text;

    DialoguePage() : text() {}
    explicit DialoguePage(std::string t) : text(std::move(t)) {}
};

// Immutable dialogue state
class DialogueState {
public:
    // Create inactive dialogue state
    static DialogueState inactive() {
        return DialogueState{};
    }

    // Create active dialogue state with pages
    static DialogueState create(std::vector<DialoguePage> pages) {
        return DialogueState{std::move(pages), 0, true};
    }

    // Advance to next page (returns new state)
    [[nodiscard]] DialogueState advance() const {
        if (!isActive_ || isLastPage()) {
            return inactive();
        }
        return DialogueState{pages_, currentPage_ + 1, true};
    }

    // Check if on last page
    [[nodiscard]] bool isLastPage() const {
        return currentPage_ >= static_cast<int>(pages_.size()) - 1;
    }

    // Get current page text
    [[nodiscard]] const std::string& getCurrentText() const {
        static const std::string empty;
        if (!isActive_ || pages_.empty()) {
            return empty;
        }
        return pages_[currentPage_].text;
    }

    // Query state
    [[nodiscard]] bool isActive() const { return isActive_; }
    [[nodiscard]] int getCurrentPage() const { return currentPage_; }
    [[nodiscard]] int getPageCount() const { return static_cast<int>(pages_.size()); }

private:
    // Private constructor for inactive state
    DialogueState() : pages_{}, currentPage_(0), isActive_(false) {}

    // Private constructor for active state
    DialogueState(std::vector<DialoguePage> pages, int page, bool active)
        : pages_(std::move(pages)), currentPage_(page), isActive_(active) {}

    std::vector<DialoguePage> pages_;
    int currentPage_;
    bool isActive_;
};

#endif // DIALOGUE_STATE_H
