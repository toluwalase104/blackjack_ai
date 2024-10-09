#include "game_assets.hpp"

using std::cout;

/* Stores all possible cards of the game */
game_assets::Deck::Deck(){
    int intCardValue, suiteID;
    for (int i = 0; i < game_assets::DECK_SIZE; ++i) {
        intCardValue = std::min(10, 1 + i % 13);
        suiteID = i / 13;

        // The minimum between 10 and the actual value is used because the maximum card value is 10 (excluding aces)
        game_assets::CardVal currentCardVal = game_assets::CardVal( intCardValue );
        game_assets::Suite currentSuite = game_assets::POSSIBLE_SUITES[suiteID];

        cards[i] = game_assets::Card(i, currentCardVal, currentSuite);
    }
}

/* Returns the cards at the ith position */
game_assets::Card game_assets::Deck::operator[](int i) const{
    return cards[i];
}

game_assets::Card::Card(){}

game_assets::Card::Card(int id, CardVal value, Suite suite) :
        id(id), value(value), suite(suite) {}

int game_assets::Card::getID() const{
    return game_assets::Card::id;
}

game_assets::CardVal game_assets::Card::getValue() const{
    return game_assets::Card::value;
}

game_assets::Suite game_assets::Card::getSuite() const{
    return game_assets::Card::suite;
}

std::ostream& operator<<(std::ostream& o, game_assets::CardVal v) {
    cout << static_cast<int>(v);
    return o;
}

std::ostream& operator<<(std::ostream& o, game_assets::Suite s) {
    cout << static_cast<char>(s);
    return o;
}

std::ostream& operator<<(std::ostream& o, game_assets::Card c) {
    cout << "{ Value = " << c.getValue() << ", Suite = " << c.getSuite() << "} ";
    return o;
}