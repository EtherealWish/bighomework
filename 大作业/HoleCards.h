#ifndef HOLE_CARDS_H
#define HOLE_CARDS_H

#include "poker.h"  
#include <stdexcept>

namespace Poker {

    class HoleCards {
    public:
        // ����յ���
        HoleCards() = default;

        // ���ƶѽ������ŵ���
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

        // ��ȡ������Ϣ��ֻ����
        const std::vector<Card>& getCards() const {
            return cards_;
        }

        // ��֤�Ƿ��ѷ���
        bool hasCards() const {
            return cards_.size() == 2;
        }

        // ��յ���
        void clear() {
            cards_.clear();
        }

        // ��ʽ����ʾ
        std::string toString() const {
            if (!hasCards()) return "No cards";
            return cards_[0].toString() + " | " + cards_[1].toString();
        }

    private:
        std::vector<Card> cards_;
    };

} 

#endif 