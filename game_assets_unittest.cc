#include <gtest/gtest.h>

#include "game_assets.hpp"

// Checks that all the constants defined in game_assets are correct
TEST(GameAssetsTests, ConstantsAreCorrect){
    EXPECT_EQ(52, game_assets::DECK_SIZE);

    EXPECT_EQ(4, game_assets::POSSIBLE_SUITES.size());

    EXPECT_EQ(game_assets::Suite::HEARTS, game_assets::POSSIBLE_SUITES[0]);
    EXPECT_EQ(game_assets::Suite::DIAMONDS, game_assets::POSSIBLE_SUITES[1]);
    EXPECT_EQ(game_assets::Suite::SPADES, game_assets::POSSIBLE_SUITES[2]);
    EXPECT_EQ(game_assets::Suite::CLUBS, game_assets::POSSIBLE_SUITES[3]);

}

// Ensures all cards in the deck are correct
TEST(GameAssetsTests, AllCardsAreCorrect){
    game_assets::Deck deck;

    for (int i = 0; i < game_assets::DECK_SIZE; ++i){
        // Recalculate the correct values
        game_assets::CardVal expectedCardValue = game_assets::CardVal(std::min(10, 1 + i % 13));
        game_assets::Suite expectedSuite = game_assets::POSSIBLE_SUITES[i / 13];

        // Get the values stored
        game_assets::CardVal actualCardValue = deck[i].getValue();
        game_assets::Suite actualSuite = deck[i].getSuite();

        EXPECT_EQ((int)expectedCardValue, (int)actualCardValue);
        EXPECT_EQ((char)expectedSuite, (char)actualSuite);
    }

}
