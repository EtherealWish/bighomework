#ifndef POKER_DECK_H
#define POKER_DECK_H

#include <vector>
#include <string>
#include <algorithm>
#include <random>

namespace Poker {

    // �˿˻�ɫö��
    enum class Suit {
        Hearts,   // ����
        Diamonds, // ����
        Clubs,    // ÷��
        Spades    // ����
    };

    // �˿˵���ö��
    enum class Rank {
        Ace = 1,
        Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten,
        Jack,  // J
        Queen, // Q
        King   // K
    };

    // �����˿�����
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
            case Suit::Hearts:   return "Hearts";//����
            case Suit::Diamonds: return "Diamonds";//��Ƭ
            case Suit::Clubs:    return "Clubs";//÷��
            case Suit::Spades:   return "Spades";//����
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

    // �����˿�����
    class Deck {
    public:
        Deck() { reset(); }

        // �����ƶѣ���˳���������ƣ�
        void reset() {
            cards_.clear();
            for (int s = 0; s < 4; ++s) {     // 4�ֻ�ɫ
                for (int r = 1; r <= 13; ++r) { // 13������
                    cards_.emplace_back(
                        static_cast<Suit>(s),
                        static_cast<Rank>(r)
                    );
                }
            }
        }

        // ϴ��
        void shuffle() {
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(cards_.begin(), cards_.end(), g);
        }

        // ��һ����
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