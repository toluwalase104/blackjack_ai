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

    const int MAX_PLAYER_TOTAL = 21, MAX_DEALER_SHOWING = 11, MAX_POSSIBLE_ACTIONS = 2, MAX_ACE_VALUE = 1;

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
        int numberOfDeals;

        GameState();

        void addCard(game_assets::Card card, bool forPlayer);

        int getPlayerTotal() const;

        int getDealerTotal() const;

        int getFaceupTotal() const;

        /* Returns all cards seen so far, combined into one set */
        std::vector<int> getSeenCards() const;
        
        std::vector<int> getPlayerCards() const;

        std::vector<int> getDealerCards() const;

        void setOutcome(GameResult outcome);

        GameResult getOutcome() const;

        bool cardSeen(game_assets::Card card);

        bool cardSeen(int cardID);

        int getNumberOfSeenCards() const;

        /* Show the cards that the dealer had facing down*/
        void showDealerCards();

        /* Check if the dealers cards are shown */
        bool dealerCardsShown();

        bool doesPlayerHaveUsableAce() const;

        bool doesDealerHaveUsableAce() const;

        void updateTotal(int cardValue, int &total, bool &usableAce);

        /* Allows the pretty printing of currently stored cards */
        std::string stringifyCards();

        bool operator==(GameState comparedState);

    private:
        std::vector<int> playerCards, dealerCards;
        int playerTotal, dealerTotal, faceupTotal, numberOfSeenCards;
        bool dealerShowsAll, playerHasUsableAce, dealerHasUsableAce;
        GameResult outcome;
    };

    class EnvironmentHandler {
    public:
        std::vector<game_assets::Card> deck;

        EnvironmentHandler();

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

        int numberOfRemainingCards;

        std::unordered_set<int> seenIDs;

    };

}

std::ostream& operator<<(std::ostream& o, environment::GameResult r);
std::ostream& operator<<(std::ostream& o, environment::GameState s);
std::ostream& operator<<(std::ostream& o, environment::Action a);

#endif /* ENVIRONMENT_H */