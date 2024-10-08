#include "environment.hpp"
#include <gtest/gtest.h>

class EnvironmentHandlerTests : public testing::Test {
    protected:
        EnvironmentHandlerTests() {}
        environment::EnvironmentHandler e0;
        game_assets::Deck deck;

        // Utility function that calculates the value of the recorded cards
        int calculateTotalCardValue(std::vector<int> &seenCards) const{
            int total = 0;
            for (int i = 0; i < game_assets::DECK_SIZE; ++i){
                if (seenCards[i]){
                    total += (int)deck[i].getValue();
                }
            }
            return total;
        }
};

class GameStateTests : public testing::Test {
    protected:
        GameStateTests(){}

    environment::GameState gs0;
    game_assets::Deck deck;
};

TEST_F(EnvironmentHandlerTests, InitialTotalsAreConsistent){

    environment::GameState s0 = e0.getCurrentState();

    // Compare the player total with respect to the cards they have
    EXPECT_EQ(calculateTotalCardValue(s0.getPlayerCards()), s0.getPlayerTotal());

    // Initially the dealer total should be equal to the face up total
    EXPECT_EQ(calculateTotalCardValue(s0.getDealerCards()), s0.getDealerTotal());
}

TEST_F(EnvironmentHandlerTests, InitialCardStatusIsCorrect){
     
    environment::GameState s0 = e0.getCurrentState();

    // Expect initial number of seen cards to be 4
    EXPECT_EQ(4, s0.getNumberOfSeenCards());

    // Expect the dealer to not show all cards initially
    EXPECT_FALSE(s0.dealerCardsShown());
}

// An ace by itself would be the lower limit, whereas an Ace with a 10 or a face card would be the upper limit of inclusion
TEST_F(GameStateTests, UsableAceIsCorrectlyIncludedLowerLimit){

    // Add the Ace of Hearts to the player's hand
    gs0.addCard(deck[0], true);

    // The useable ace should be flagged and the ace's value should be 11
    EXPECT_TRUE(gs0.doesPlayerHaveUsableAce());

    EXPECT_EQ(11, gs0.getPlayerTotal());

}

// When all aces are in a players deck it should be correctly handled in the game state
TEST_F(GameStateTests, UsableAceIsCorrectlyIncludedWithAllAces){
    // Add the Ace of Hearts to the player's hand
    gs0.addCard(deck[0], true);

    // Add the Ace of Diamonds to the player's hand
    gs0.addCard(deck[13], true);

    // Add the Ace of Clubs to the player's hand
    gs0.addCard(deck[26], true);

    // Add the Ace of Hearts to the player's hand
    gs0.addCard(deck[39], true);

    // The useable ace should be flagged and the ace's value should be 14
    EXPECT_TRUE(gs0.doesPlayerHaveUsableAce());

    EXPECT_EQ(14, gs0.getPlayerTotal());

}

// An ace by itself would be the lower limit, whereas an Ace with a 10 or a face card would be the upper limit of inclusion
TEST_F(GameStateTests, UsableAceIsCorrectlyIncludedUpperLimit){
    // Add the Ace of Hearts to the player's hand
    gs0.addCard(deck[0], true);

    // Add the King of Hearts to the player's hand
    gs0.addCard(deck[12], true);

    // The useable ace should be flagged
    EXPECT_TRUE(gs0.doesPlayerHaveUsableAce());

    // The player total should be 11 + 10 = 21
    EXPECT_EQ(21, gs0.getPlayerTotal());

}

// The lowest limit of exclusion would be an Ace added to a hand of value 11
TEST_F(GameStateTests, UsableAceIsCorrectlyExcludedLowerLimit){

    // Add the Ace of Hearts to the player's hand
    gs0.addCard(deck[0], true);

    // Add the Five of Hearts to the player's hand
    gs0.addCard(deck[4], true);

    // Add the Six of Hearts to the player's hand
    gs0.addCard(deck[5], true);    

    // The ace should be flagged as unusable so as to not go bust
    EXPECT_FALSE(gs0.doesPlayerHaveUsableAce());

    // The player's total should be equal to 12
    EXPECT_EQ(12, gs0.getPlayerTotal());
    
}

// When all aces are in a players deck it should be correctly handled in the game state
TEST_F(GameStateTests, UsableAceIsCorrectlyExcludedWithAllAces){
    // Add the Seven of Hearts to the player's hand
    gs0.addCard(deck[6], true);

    // Add the Ten of Hearts to the player's hand
    gs0.addCard(deck[9], true);

    // Add the Ace of Hearts to the player's hand
    gs0.addCard(deck[0], true);

    // Add the Ace of Diamonds to the player's hand
    gs0.addCard(deck[13], true);

    // Add the Ace of Clubs to the player's hand
    gs0.addCard(deck[26], true);

    // Add the Ace of Hearts to the player's hand
    gs0.addCard(deck[39], true);

    // The useable ace should be flagged and the ace's value should be 14
    EXPECT_FALSE(gs0.doesPlayerHaveUsableAce());

    EXPECT_EQ(21, gs0.getPlayerTotal());

}

// The lowest limit of exclusion would be an Ace added to a hand of value 11
TEST_F(GameStateTests, UsableAceIsCorrectlyExcludedUpperLimit){

    // Add the Ace of Hearts to the player's hand
    gs0.addCard(deck[0], true);

    // Add the Queen of Hearts to the player's hand
    gs0.addCard(deck[11], true);

    // Add the King of Hearts to the player's hand
    gs0.addCard(deck[12], true);    

    // The ace should be flagged as unusable so as to not go bust
    EXPECT_FALSE(gs0.doesPlayerHaveUsableAce());

    // The player's total should be equal to 21
    EXPECT_EQ(21, gs0.getPlayerTotal());
    
}