#ifndef HOLE_CARDS_H
#define HOLE_CARDS_H

#include "poker.h"  
#include <stdexcept>

namespace Poker {

    class HoleCards {
    public:
        // 构造空底牌
        HoleCards() = default;

        // 从牌堆接收两张底牌
        void receiveCards(Deck& deck) {
            if (cards_.size() >= 2) {
                throw std::logic_error("Hole cards already received");
            }

            try {
                cards_.push_back(deck.deal());
                cards_.push_back(deck.deal());
            }
            catch (const std::out_of_range&) {
                throw std::runtime_error("Not enough cards in deck");
            }
        }

        // 获取底牌信息（只读）
        const std::vector<Card>& getCards() const {
            return cards_;
        }

        // 验证是否已发牌
        bool hasCards() const {
            return cards_.size() == 2;
        }

        // 清空底牌
        void clear() {
            cards_.clear();
        }

        // 格式化显示
        std::string toString() const {
            if (!hasCards()) return "No cards";
            return cards_[0].toString() + " | " + cards_[1].toString();
        }

    private:
        std::vector<Card> cards_;
    };

} 

#endif 