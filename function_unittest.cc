#include <gtest/gtest.h>

#include <numeric>

#include "function.hpp"

class FunctionTests : public testing::Test {
    protected:
        FunctionTests(){}

        function::StateActionFunction func0;
        game_assets::Deck deck;
};


TEST_F(FunctionTests, StoresCorrectValueWithUsableAce){
    environment::GameState state;

    // Gives the dealer the Six of Hearts
    state.addCard( deck[5], false );

    // Gives the player the Ace of Diamonds
    state.addCard( deck[13], true );

    // Gives the player the Seven of Diamonds
    state.addCard( deck[19], true );

    // Expects the state and action to return the same pointer as the translated image
    EXPECT_EQ(func0.getImage(18, 6, 1, 1), func0(state, environment::Action::HIT));

}