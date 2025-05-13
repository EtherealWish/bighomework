#include <graphics.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <string>
#include "ks.h"
#include "poker.h"
#include "HoleCards.h"
#include "texas_holdem_evaluator.h"

using namespace Poker;
using namespace rules;

const int WIDTH = 1500;
const int HEIGHT = 900;

class PokerGame {
private:
    Deck deck;
    HoleCards playerHand;
    HoleCards computerHand;
    std::vector<Card> communityCards;
    int pot = 0;
    int playerChips = 1000;
    int computerChips = 1000;
    bool gameEnd = false;

private:
    // 辅助函数：将牌面转换为图片的文件名
    TCHAR* getCardImageName(const Card& card) {
        static TCHAR filename[50];

        // 转换花色
        const TCHAR* suitStr;
        switch (card.suit()) {
        case Suit::Hearts:   suitStr = _T("heart");   break;
        case Suit::Diamonds: suitStr = _T("diamond"); break;
        case Suit::Clubs:    suitStr = _T("club");    break;
        case Suit::Spades:   suitStr = _T("spade");   break;
        default:             suitStr = _T("unknown");
        }

        // 转换点数
        const TCHAR* rankStr;
        switch (card.rank()) {
        case Rank::Ace:   rankStr = _T("A");  break;
        case Rank::Two:   rankStr = _T("2");  break;
        case Rank::Three: rankStr = _T("3");  break;
        case Rank::Four:  rankStr = _T("4");  break;
        case Rank::Five:  rankStr = _T("5");  break;
        case Rank::Six:   rankStr = _T("6");  break;
        case Rank::Seven: rankStr = _T("7");  break;
        case Rank::Eight: rankStr = _T("8");  break;
        case Rank::Nine:  rankStr = _T("9");  break;
        case Rank::Ten:   rankStr = _T("10"); break;
        case Rank::Jack:  rankStr = _T("J");  break;
        case Rank::Queen: rankStr = _T("Q");  break;
        case Rank::King:  rankStr = _T("K");  break;
        default:          rankStr = _T("?");  
        }

        _stprintf_s(filename, _T("%s%s.png"), suitStr, rankStr);
        return filename;
    }

    void drawCard(int x, int y, const Card& card, bool faceUp = true) {
        IMAGE cardImg;
        TCHAR imgPath[MAX_PATH] = { 0 };

        // 生成图片路径
        if (faceUp) {
            _stprintf_s(imgPath, _T("%s%s"),
                CARD_IMAGE_BASE_PATH,
                getCardImageName(card));
        }
        else {
            _stprintf_s(imgPath, _T("%s%s"),
                CARD_IMAGE_BASE_PATH,
                CARD_BACK_IMAGE);
        }

        // 加载图片并检查结果
        bool loadSuccess = false;

#ifdef _EASYX_VER_ // 新版检测
        loadSuccess = loadimage(&cardImg, imgPath, CARD_WIDTH, CARD_HEIGHT);
#else              // 旧版始终返回成功
        loadimage(&cardImg, imgPath, CARD_WIDTH, CARD_HEIGHT);
        loadSuccess = true;
#endif

        if (loadSuccess) {
            putimage(x, y, &cardImg);
        }
        else {
            drawErrorCard(x, y, _T("Load Failed"));
        }
    }

    // 错误卡片绘制函数
    void drawErrorCard(int x, int y, LPCTSTR msg) {
        setfillcolor(HSVtoRGB(0, 0.8, 1)); // 亮红色
        fillrectangle(x, y, x + CARD_WIDTH, y + CARD_HEIGHT);

        settextcolor(BLACK);
        settextstyle(14, 0, _T("Consolas"));
        outtextxy(x + 5, y + 5, msg);
    }
    const TCHAR* suitSymbol(Suit s) {
        switch (s) {
        case Suit::Hearts: return _T("♥");
        case Suit::Diamonds: return _T("♦");
        case Suit::Clubs: return _T("♣");
        case Suit::Spades: return _T("♠");
        default: return _T("");
        }
    }

public:
    void run() {
        initgraph(WIDTH, HEIGHT);
        BeginBatchDraw();

        // 加载背景
        IMAGE bgImg;
        loadimage(&bgImg, _T("D:\\大作业\\Source\\background.jpg"), WIDTH, HEIGHT);

        deck.shuffle();
        dealInitialCards();

        while (!gameEnd) {
            cleardevice();
            putimage(0, 0, &bgImg);

            // 绘制筹码信息
            settextcolor(WHITE);
            TCHAR chipInfo[100];
            _stprintf_s(chipInfo, _T("玩家筹码: $%d  底池: $%d  电脑筹码: $%d"),
                playerChips, pot, computerChips);
            outtextxy(50, 50, chipInfo);

            // 绘制电脑手牌（背面）
            drawCard(600, 100, Card(Suit::Spades, Rank::Ace), false);
            drawCard(800, 100, Card(Suit::Hearts, Rank::King), false);

            // 绘制公共牌
            int startX = 400;
            for (size_t i = 0; i < communityCards.size(); i++) {
                drawCard(startX + i * 150, 300, communityCards[i]);
            }

            // 绘制玩家手牌
            if (playerHand.hasCards()) {
                drawCard(600, 600, playerHand.getCards()[0]);
                drawCard(800, 600, playerHand.getCards()[1]);
            }

            // 绘制操作按钮
            setfillcolor(RGB(70, 130, 180));
            fillroundrect(200, 750, 400, 830, 10, 10); // 跟注
            fillroundrect(450, 750, 650, 830, 10, 10); // 加注
            fillroundrect(700, 750, 900, 830, 10, 10); // 弃牌

            settextcolor(WHITE);
            outtextxy(250, 770, _T("跟注 ($50)"));
            outtextxy(500, 770, _T("加注"));
            outtextxy(750, 770, _T("弃牌"));

            FlushBatchDraw();

            processInput();
        }

        EndBatchDraw();
        closegraph();
    }

private:
    void dealInitialCards() {
        playerHand.receiveCards(deck);
        computerHand.receiveCards(deck);
        pot = 100;  // 盲注
        playerChips -= 50;
        computerChips -= 50;
    }

    void processInput() {
        if (MouseHit()) {
            MOUSEMSG msg = GetMouseMsg();
            if (msg.uMsg == WM_LBUTTONDOWN) {
                // 处理跟注
                if (msg.x > 200 && msg.x < 400 && msg.y > 750 && msg.y < 830) {
                    playerChips -= 50;
                    pot += 50;
                    computerAction();
                    nextStage();
                }
                // 处理加注
                else if (msg.x > 450 && msg.x < 650 && msg.y > 750 && msg.y < 830) {
                    // 简化加注逻辑
                    playerChips -= 100;
                    pot += 100;
                    computerAction();
                    nextStage();
                }
                // 处理弃牌
                else if (msg.x > 700 && msg.x < 900 && msg.y > 750 && msg.y < 830) {
                    gameEnd = true;
                    showResult(true);
                }
            }
        }
    }

    void computerAction() {
        // 简单AI逻辑：80%跟注，20%加注
        int action = rand() % 10;
        if (action < 8) {
            computerChips -= 50;
            pot += 50;
        }
        else {
            computerChips -= 100;
            pot += 100;
        }
    }

    void nextStage() {
        static int stage = 0;
        switch (stage++) {
        case 0: dealCommunityCards(3); break; // 翻牌
        case 1: dealCommunityCards(1); break; // 转牌
        case 2: dealCommunityCards(1);        // 河牌
            showResult(false);
            gameEnd = true;
            break;
        }
    }

    void dealCommunityCards(int num) {
        for (int i = 0; i < num; i++) {
            communityCards.push_back(deck.deal());
        }
    }

    void showResult(bool folded) {
        cleardevice();

        // 显示所有牌
        drawCard(600, 100, computerHand.getCards()[0]);
        drawCard(800, 100, computerHand.getCards()[1]);

        // 胜负判定
        std::vector<std::vector<Card>> hands = {
            playerHand.getCards(),
            computerHand.getCards()
        };

        auto winners = Evaluator::determineWinners(hands, communityCards);

        settextcolor(WHITE);
        if (folded) {
            outtextxy(600, 400, _T("玩家弃牌，电脑获胜!"));
        }
        else if (winners[0] == 0) {
            outtextxy(600, 400, _T("玩家获胜!"));
            playerChips += pot;
        }
        else {
            outtextxy(600, 400, _T("电脑获胜!"));
            computerChips += pot;
        }

        FlushBatchDraw();
        Sleep(3000);
    }
};

int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    PokerGame game;
    game.run();
    return 0;
}