#pragma once

#ifndef GAME_ASSETS_H

#define GAME_ASSETS_H

#include <iostream>

namespace game_assets{
    /*  Currently Aces are treated as just 1 so additional features
        need to be implemented to handle usable Aces. */
    enum class CardVal :int {
        Ace = 1,
        Two = 2, Three = 3, Four = 4,
        Five = 5, Six = 6, Seven = 7,
        Eight = 8, Nine = 9, Ten = 10,
        Jack = 10, Queen = 10, King = 10
    };//card_value;

    enum class Suite :char {
        Hearts = 'H',
        Diamonds = 'D',
        Spades = 'S',
        Clubs = 'C'
    };//card_suite;

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