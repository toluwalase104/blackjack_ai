#pragma once

#ifndef GAME_ASSETS_H

#define GAME_ASSETS_H

#include <iostream>

namespace game_assets{
    /*  Currently Aces are treated as just 1 so additional features
        need to be implemented to handle usable Aces. */
    enum class CardVal :int {
        ACE = 1,
        TWO = 2, THREE = 3, FOUR = 4,
        FIVE = 5, SIX = 6, SEVEN = 7,
        EIGHT = 8, NINE = 9, TEN = 10,
        JACK = 10, QUEEN = 10, KING = 10
    };

    enum class Suite :char {
        HEARTS = 'H',
        DIAMONDS = 'D',
        SPADES = 'S',
        CLUBS = 'C'
    };

    class Card {
    private:
        int id;
        CardVal value;
        Suite suite;
    public:
        Card();

        Card(int id, CardVal value, Suite suite);

        int getID() const;

        CardVal getValue() const;

        Suite getSuite() const;
    };

}

std::ostream& operator<<(std::ostream& o, game_assets::Card c);

std::ostream& operator<<(std::ostream& o, game_assets::CardVal v);

std::ostream& operator<<(std::ostream& o, game_assets::Suite s);

#endif /* GAME_ASSETS_H */