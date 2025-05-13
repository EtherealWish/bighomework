#ifndef POKER_DECK_H
#define POKER_DECK_H

#include <vector>
#include <string>
#include <algorithm>
#include <random>

namespace Poker {

    // 扑克花色枚举
    enum class Suit {
        Hearts,   // 红心
        Diamonds, // 方块
        Clubs,    // 梅花
        Spades    // 黑桃
    };

    // 扑克点数枚举
    enum class Rank {
        Ace = 1,
        Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten,
        Jack,  // J
        Queen, // Q
        King   // K
    };

    // 单张扑克牌类
    class Card {
    public:
        Card(Suit s, Rank r) : suit_(s), rank_(r) {}

        Suit suit() const { return suit_; }
        Rank rank() const { return rank_; }

        std::string toString() const {
            return rankString() + " of " + suitString();
        }

    private:
        Suit suit_;
        Rank rank_;

        std::string suitString() const {
            switch (suit_) {
            case Suit::Hearts:   return "Hearts";//红桃
            case Suit::Diamonds: return "Diamonds";//方片
            case Suit::Clubs:    return "Clubs";//梅花
            case Suit::Spades:   return "Spades";//黑桃
            default:             return "Unknown";
            }
        }

        std::string rankString() const {
            switch (rank_) {
            case Rank::Ace:   return "Ace";//A
            case Rank::Jack:  return "Jack";//J
            case Rank::Queen: return "Queen";//Q
            case Rank::King:  return "King";//K
            default:
                if (rank_ >= Rank::Two && rank_ <= Rank::Ten) {
                    return std::to_string(static_cast<int>(rank_));
                }
                return "Unknown";
            }
        }
    };

    // 整副扑克牌类
    class Deck {
    public:
        Deck() { reset(); }

        // 重置牌堆（按顺序生成新牌）
        void reset() {
            cards_.clear();
            for (int s = 0; s < 4; ++s) {     // 4种花色
                for (int r = 1; r <= 13; ++r) { // 13个点数
                    cards_.emplace_back(
                        static_cast<Suit>(s),
                        static_cast<Rank>(r)
                    );
                }
            }
        }

        // 洗牌
        void shuffle() {
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(cards_.begin(), cards_.end(), g);
        }

        // 发一张牌
        Card deal() {
            if (isEmpty()) {
                throw std::out_of_range("Deck is empty");
            }
            Card top = cards_.back();
            cards_.pop_back();
            return top;
        }

        bool isEmpty() const { return cards_.empty(); }
        size_t size() const { return cards_.size(); }

    private:
        std::vector<Card> cards_;
    };

} 

#endif 