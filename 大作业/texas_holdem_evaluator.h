#ifndef TEXAS_HOLDEM_EVALUATOR_H
#define TEXAS_HOLDEM_EVALUATOR_H

#include <vector>
#include <algorithm>
#include <map>
#include "poker.h"  // ����֮ǰ���˿��ƶ���
#include"HoleCards.h"
using namespace Poker;
namespace rules{

    // ���͵ȼ�ö��
    enum class HandRank {
        HIGH_CARD,      // ����
        ONE_PAIR,       // һ��
        TWO_PAIR,       // ����
        THREE_OF_A_KIND,// ����
        STRAIGHT,       // ˳��
        FLUSH,         // ͬ��
        FULL_HOUSE,    // ��«
        FOUR_OF_A_KIND, // ����
        STRAIGHT_FLUSH, // ͬ��˳
        ROYAL_FLUSH     // �ʼ�ͬ��˳
    };

    // ���ͷ������
    struct HandStrength {
        HandRank rank;
        std::vector<Rank> kickers;

        bool operator>(const HandStrength& other) const {
            if (rank != other.rank)
                return rank > other.rank;
            for (size_t i = 0; i < kickers.size(); ++i) {
                if (kickers[i] != other.kickers[i])
                    return kickers[i] > other.kickers[i];
            }
            return false;
        }

        bool operator<(const HandStrength& other) const {
            if (rank != other.rank)
                return rank < other.rank;  
            for (size_t i = 0; i < kickers.size(); ++i) {
                if (kickers[i] != other.kickers[i])
                    return kickers[i] < other.kickers[i];
            }
            return false;
        }
    };

    class Evaluator {
    public:

        static HandStrength evaluateHand(const std::vector<Card>& cards) {
            std::vector<std::vector<Card>> combinations = generateCombinations(cards, 5);
            HandStrength best;
            for (auto& combo : combinations) {
                HandStrength current = analyzeCombo(combo);
                if (current > best) best = current;
            }
            return best;
        }

        static std::vector<int> determineWinners(
            const std::vector<std::vector<Card>>& allHands, 
            const std::vector<Card>& communityCards
        ) {
            std::vector<HandStrength> strengths;
            for (const auto& hand : allHands) {  // ʹ��const���ñ���
                auto fullHand = hand;  // ��������
                fullHand.insert(fullHand.end(), communityCards.begin(), communityCards.end());
                strengths.push_back(evaluateHand(fullHand));
            }

            std::vector<int> winners;
            HandStrength maxStrength = *std::max_element(strengths.begin(), strengths.end());

            for (size_t i = 0; i < strengths.size(); ++i) {
                if (!(strengths[i] < maxStrength)) {
                    winners.push_back(i);
                }
            }
            return winners;
        }


    private:
        // �������ͷ���
        static HandStrength analyzeCombo(const std::vector<Card>& cards) {
            sortByRank(cards);

            bool isFlush = checkFlush(cards);
            bool isStraight = checkStraight(cards);
            auto counts = countRanks(cards);

            HandStrength result;

            // �ʼ�ͬ��˳��ͬ�� + ˳�� + ����Ϊ10��
            if (isFlush && isStraight && cards[0].rank() == Rank::Ten) {
                result.rank = HandRank::ROYAL_FLUSH;
                result.kickers = { Rank::Ace };
                return result;
            }

            // ͬ��˳
            if (isFlush && isStraight) {
                result.rank = HandRank::STRAIGHT_FLUSH;
                result.kickers = { cards[0].rank() };
                return result;
            }

            // ����
            if (counts[0].second == 4) {
                result.rank = HandRank::FOUR_OF_A_KIND;
                result.kickers = { counts[0].first, counts[1].first };
                return result;
            }

            // ��«
            if (counts[0].second == 3 && counts[1].second == 2) {
                result.rank = HandRank::FULL_HOUSE;
                result.kickers = { counts[0].first, counts[1].first };
                return result;
            }

            // ͬ��
            if (isFlush) {
                result.rank = HandRank::FLUSH;
                for (const auto& c : cards) result.kickers.push_back(c.rank());
                return result;
            }

            // ˳��
            if (isStraight) {
                result.rank = HandRank::STRAIGHT;
                result.kickers = { cards[0].rank() };
                return result;
            }

            // ����
            if (counts[0].second == 3) {
                result.rank = HandRank::THREE_OF_A_KIND;
                result.kickers = { counts[0].first, counts[1].first, counts[2].first };
                return result;
            }

            // ����
            if (counts[0].second == 2 && counts[1].second == 2) {
                result.rank = HandRank::TWO_PAIR;
                result.kickers = { counts[0].first, counts[1].first, counts[2].first };
                return result;
            }

            // һ��
            if (counts[0].second == 2) {
                result.rank = HandRank::ONE_PAIR;
                result.kickers = { counts[0].first, counts[1].first, counts[2].first, counts[3].first };
                return result;
            }

            // ����
            result.rank = HandRank::HIGH_CARD;
            for (const auto& c : cards) result.kickers.push_back(c.rank());
            return result;
        }

        // ��������
        static void sortByRank(std::vector<Card> cards) {
            std::sort(cards.rbegin(), cards.rend(), [](const Card& a, const Card& b) {
                return static_cast<int>(a.rank()) < static_cast<int>(b.rank());
                });
        }

        static bool checkFlush(const std::vector<Card>& cards) {
            Suit s = cards[0].suit();
            for (const auto& c : cards) {
                if (c.suit() != s) return false;
            }
            return true;
        }

        static bool checkStraight(std::vector<Card> cards) {
            // ����A-2-3-4-5�������
            bool hasAce = cards[0].rank() == Rank::Ace;
            if (hasAce) {
                bool lowStraight = true;
                for (int i = 1; i < 5; ++i) {
                    if (cards[i].rank() != static_cast<Rank>(5 - i)) {
                        lowStraight = false;
                        break;
                    }
                }
                if (lowStraight) return true;
            }

            for (size_t i = 0; i < cards.size() - 1; ++i) {
                if (static_cast<int>(cards[i].rank()) - 1 !=
                    static_cast<int>(cards[i + 1].rank())) {
                    return false;
                }
            }
            return true;
        }

        static std::vector<std::pair<Rank, int>> countRanks(const std::vector<Card>& cards) {
            std::map<Rank, int> counter;
            for (const auto& c : cards) {
                counter[c.rank()]++;
            }

            std::vector<std::pair<Rank, int>> result;
            for (const auto& p : counter) {
                result.emplace_back(p.first, p.second);
            }

            std::sort(result.rbegin(), result.rend(),
                [](const auto& a, const auto& b) {
                    if (a.second != b.second) return a.second < b.second;
                    return static_cast<int>(a.first) < static_cast<int>(b.first);
                });

            return result;
        }

        // ������ɣ�nѡk��
        static std::vector<std::vector<Card>> generateCombinations(
            const std::vector<Card>& cards, int k)
        {
            std::vector<std::vector<Card>> result;
            std::vector<bool> mask(cards.size(), false);
            std::fill(mask.begin(), mask.begin() + k, true);

            do {
                std::vector<Card> combo;
                for (size_t i = 0; i < cards.size(); ++i) {
                    if (mask[i]) combo.push_back(cards[i]);
                }
                result.push_back(combo);
            } while (std::prev_permutation(mask.begin(), mask.end()));

            return result;
        }
    };

} 

#endif 