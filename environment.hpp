#pragma once

#ifndef ENVIRONMENT_H

#define ENVIRONMENT_H

#include <set>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include "game_assets.hpp"

namespace environment {
    const int DECK_SIZE = 52;

    const int MAX_PLAYER_TOTAL = 21, MAX_DEALER_SHOWING = 10, MAX_POSSIBLE_ACTIONS = 2;

    const std::string POSSIBLE_SUITES = "HDSC";

    inline float getRandomFloat() {
        return (float)(rand()) / (float)(RAND_MAX);
    }

    /*  The GameResult determines the reward the agent receives
        Mutual bust and push happen when the player and dealer both lose or both win respecetively
        Consider testing out what happens if you set mutaul bust and push equal to 0 or playing around with the reward values */
    enum class GameResult :int {
        DEALER_WIN = -2,     // Player lost
        MUTUAL_BUST = -1,    // Mutual loss
        UNFINISHED = 0,     // Game unfinished
        PUSH = 1,           // Mutual "win"
        PLAYER_WIN = 2       // Player won
    };

    /* The possible actions an agent can make */
    enum class Action: bool{
        STAND = false,
        HIT = true
    };

    /* Stores the essential data used by the model to determine the game's state */
    class GameState {
    public:
        GameState();

        void addCard(game_assets::Card card, bool forPlayer);

        int getPlayerTotal() const;

        int getDealerTotal() const;

        int getFaceupTotal() const;

        /* Returns all cards seen so far, combined into one set */
        std::vector<int> getSeenCards() const;

        void setOutcome(GameResult outcome);

        GameResult getOutcome() const;

        bool cardSeen(game_assets::Card card);

        bool cardSeen(int cardID);

        int getNumberOfSeenCards() const;

        void showFacedown();

        bool allCardsFaceup();
        
        /* Allows the pretty printing of currently stored cards */
        std::string stringifyCards();

        bool operator==(GameState comparedState);

    private:
        std::vector<int> playerCards, dealerCards;
        int playerTotal, dealerTotal, faceupTotal, numberOfSeenCards;
        bool dealerShowsAll;
        GameResult outcome;
    };

    class EnvironmentHandler {
    public:
        EnvironmentHandler();

        /* Selects the index of an unseen card */
        int selectRandomIndex();

        /* Selects a card that has not been seen yet */
        int selectOutOfRemainingCards();

        /* Generates the required number of cards for the current game state */
        std::vector<game_assets::Card> getNextHand();

        void updateTotals(std::vector<game_assets::Card> cardsDealt, int numberOfCards);

        GameResult checkGameResult();

        GameResult simulateNextRound(Action action);

        GameState getCurrentState() const;

    private:
        GameState currentState;

        std::vector<game_assets::Card> deck;

        std::vector<game_assets::Card> currentHand;

        int numberOfDeals;
        std::unordered_set<int> seenCards;

        // Player initially always hits when they choose stand
        // Their choice is locked until the end of the episode
        bool playerChoseToHit;
        int dealerTotal, playerTotal, faceupTotal;
    };

}

std::ostream& operator<<(std::ostream& o, environment::GameResult r);
std::ostream& operator<<(std::ostream& o, environment::GameState s);
std::ostream& operator<<(std::ostream& o, environment::Action a);

#endif /* ENVIRONMENT_H */