#include <iostream>
#include <cstdlib>
#include <chrono>
#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>

using namespace std::chrono;
using std::vector;
using std::cout;

enum class CardVal :int {
    Ace = 1,
    Two = 2, Three = 3, Four = 4,
    Five = 5, Six = 6, Seven = 7,
    Eight = 8, Nine = 9, Ten = 10,
    Jack = 11, Queen = 12, King = 13
}card_value;

std::ostream& operator<<(std::ostream& o, CardVal v) {
    cout << static_cast<int>(v);
    return o;
}

enum class Suite :char {
    Hearts = 'H',
    Diamonds = 'D',
    Spades = 'S',
    Clubs = 'C'
}card_suite;

std::ostream& operator<<(std::ostream& o, Suite s) {
    cout << static_cast<char>(s);
    return o;
}

class Card {
private:
    CardVal value;
    Suite suite;
public:
    Card() {

    }

    Card(CardVal value, Suite suite) :
        value(value), suite(suite) {
    }

    CardVal getValue() {
        return value;
    }

    Suite getSuite() {
        return suite;
    }

};

std::ostream& operator<<(std::ostream& o, Card c) {
    cout << "{ Value = " << c.getValue() << ", Suite = " << c.getSuite() << "} ";
    return o;
}

class Environment {
private:
    const int DECK_SIZE = 52;
    vector<Card> deck;

    vector<Card> currentHand;

    int numberOfDeals;
    std::unordered_set<int> seenCards;

public:
    Environment() {
        srand(time(0));
        numberOfDeals = 0;

        // Defines the amount of space the deck will need and will take
        deck.reserve(60);
        deck.resize(52);

        std::string possibleSuites = "HDSC";

        for (int i = 0; i < DECK_SIZE; ++i) {
            CardVal currentCardVal = CardVal(1 + i % 13);
            Suite currentSuite = Suite(possibleSuites[i / 13]);
            deck[i] = Card(currentCardVal, currentSuite);
            cout << "The " << i + 1 << "th card is -> " << deck[i] << "\n";
        }
        cout << "\n";

        currentHand = deal();

        cout << "Initial hand consists of " << currentHand.size() << " cards\n";
        cout << "First card of initial hand is -> " << currentHand[0] << "\n";
        cout << "Second card of initial hand is -> " << currentHand[1] << "\n";
        cout << "\n";

        for (int x : seenCards) {
            cout << "The " << x + 1 << "th card has been seen.\n";
            cout << "It was " << deck[x] << "\n";
        }

    }

    /* Selects the index of an unseen card */
    int selectRandomIndex() {
        // Calculate the number of unseen cards
        int remainingCards = DECK_SIZE - (int)seenCards.size();

        // Choose one of the cards from the unseen cards at random
        return (rand() % remainingCards) + 1;
    }

    /* Selects a card that has not been seen yet */
    int selectOutOfRemainingCards() {
        // Choose an index out of the remaining cards to select
        int index = selectRandomIndex();

        // Search for the chosen index
        for (int i = 0; i < DECK_SIZE; ++i) {
            // If the current card has been seen skip it
            if (seenCards.find(i) != seenCards.end()) {
                continue;
            }

            if (index <= 1) {
                seenCards.emplace(i);
                return i;
            }
            // Decrease the index so it can be found in the next iteration
            --index;
        }
        // Throw an exception instead of printing
        cout << "Out of cards\n";
        return 0;
    }

    /* Generates the required number of cards for the current game state */
    vector<Card> deal() {
        vector<Card> cardsDealt;
        vector<int> indicesChosen;

        // If this is the first deal then 4 cards are chosen; 2 for the dealer and 2 for the player
        // Otherwise 2 cards are chosen with one card for the dealer and one card for the player
        int numberOfCards = this->numberOfDeals == 0 ? 4 : 2;

        // Select some number of cards randomly
        int currentIndex;
        for (int i = 0; i < numberOfCards; ++i) {
            currentIndex = selectOutOfRemainingCards();

            indicesChosen.emplace_back(currentIndex);
        }

        std::transform(indicesChosen.begin(), indicesChosen.end(), std::back_inserter(cardsDealt),
            [&](int i) {
                return deck[i];
            }
        );

        ++numberOfDeals;

        return cardsDealt;
    }
};

// The initial agent 
class Agent {
private:

public:

};

int main() {
    Environment testEnvironment;

    return 0;
}