#include <iostream>
#include <cstdlib>
#include <chrono>
#include <string>
#include <vector>
#include <unordered_set>
#include <set>
#include <algorithm>
#include <thread>
#include <utility>

using namespace std::chrono;
using std::vector;
using std::cout;


std::string POSSIBLE_SUITES = "HDSC";

float getRandomFloat() {
    return (float)(rand()) / (float)(RAND_MAX);
}

/*  Currently Aces are treated as just 1 so additional features
    need to be implemented to handle usable Aces. */
enum class CardVal :int {
    Ace = 1,
    Two = 2, Three = 3, Four = 4,
    Five = 5, Six = 6, Seven = 7,
    Eight = 8, Nine = 9, Ten = 10,
    Jack = 10, Queen = 10, King = 10
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
    int id;
    CardVal value;
    Suite suite;
public:
    Card() {

    }

    Card(int id, CardVal value, Suite suite) :
        id(id), value(value), suite(suite) {
    }

    int getID() {
        return id;
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

// The GameResult determines the reward the agent receives
// Mutual bust and push happen when the player and dealer both lose or both win respecetively
// Consider testing out what happens if you set mutaul bust and push equal to 0 or playing around with the reward values
enum class GameResult :int {
    DealerWin = -2,     // Player lost
    MutualBust = -1,    // Mutual loss
    Unfinished = 0,     // Game unfinished
    Push = 1,           // Mutual "win"
    PlayerWin = 2       // Player won
}game_state;

std::ostream& operator<<(std::ostream& o, GameResult r) {
    switch (r) {
    case GameResult::DealerWin:
        cout << "The Dealer won\n";
        break;
    case GameResult::MutualBust:
        cout << "Player and Dealer busted\n";
        break;
    case GameResult::Unfinished:
        cout << "The game is unfinished\n";
        break;
    case GameResult::Push:
        cout << "Equal non-bust score\n";
        break;
    case GameResult::PlayerWin:
        cout << "The Player won\n";
        break;
    default:
        cout << "GameResult undefined\n";
    }
    return o;
}

/* Stores the essential data used by the model to determine the game's state */
class GameState {
private:
    std::set<int> playerCards, dealerCards;
    int playerTotal, dealerTotal;
    GameResult outcome;
public:
    GameState() :
        playerTotal(0), dealerTotal(0), outcome(GameResult::Unfinished) {
    }

    void addCard(Card card, bool forPlayer) {
        int cardID = card.getID();

        // If seen do not add again
        if (cardSeen(cardID)) {
            return;
        }

        int cardValue = (int)card.getValue();

        // Give card to the appropriate owner
        if (forPlayer) {
            playerCards.emplace(cardID);
            playerTotal += cardValue;
        } else {
            dealerCards.emplace(cardID);
            dealerTotal += cardValue;
        }
    }

    int getPlayerTotal() {
        return playerTotal;
    }

    int getDealerTotal() {
        return dealerTotal;
    }

    /* Returns all cards seen so far, combine into one set */
    std::set<int> getSeenCards() {
        std::set<int> result(playerCards.begin(), playerCards.end());

        result.insert(dealerCards.begin(), dealerCards.end());

        return result;
    }

    void setOutcome(GameResult outcome) {
        this->outcome = outcome;
    }
    GameResult getOutcome() {
        return outcome;
    }

    bool cardSeen(Card card) {
        int cardID = card.getID();
        return (playerCards.find(cardID) != playerCards.end()) ||
            (dealerCards.find(cardID) != dealerCards.end());
    }

    bool cardSeen(int cardID) {
        return (playerCards.find(cardID) != playerCards.end()) ||
            (dealerCards.find(cardID) != dealerCards.end());
    }

    int getNumberOfSeenCards() {
        return dealerCards.size() + playerCards.size();
    }

    /* Allows the pretty printing of currently stored cards */
    std::string stringifyCards() {
        std::string playerCardStrings = "  Player cards = {\n",
            dealerCardStrings = "  Dealer cards = {\n";

        auto convertSuite = [&](int i) {
            switch (POSSIBLE_SUITES[i / 13]) {
            case 'H':
                return " of Hearts";
            case 'D':
                return " of Diamonds";
            case 'S':
                return " of Spades";
            case 'C':
                return " of Clubs";
            default:
                return " Suite was undefined ";
            }
            };

        auto convertFaceValue = [](int i) {
            switch (i % 13) {
            case 0:
                return std::string("Ace");
            case 10:
                return std::string("Jack");
            case 11:
                return std::string("Queen");
            case 12:
                return std::string("King");
            default:
                return std::to_string(1 + i % 13);
            }
            };
        for (int i : playerCards) {
            playerCardStrings += "    " + convertFaceValue(i) + convertSuite(i) + "\n";
        }
        playerCardStrings += "  }\n";
        for (int i : dealerCards) {
            dealerCardStrings += "    " + convertFaceValue(i) + convertSuite(i) + "\n";
        }
        dealerCardStrings += "  }\n";

        return playerCardStrings + dealerCardStrings;
    }

    bool operator==(GameState comparedState) {
        return this->getSeenCards() == comparedState.getSeenCards();
    }

};

std::ostream& operator<<(std::ostream& o, GameState s) {
    cout << "GameState :{\n" <<
        "  Player Total = " << s.getPlayerTotal() << "\n" <<
        "  Dealer Total = " << s.getDealerTotal() << "\n" <<
        s.stringifyCards() << "}\n\n";
    return o;
}

class Environment {
private:
    GameState currentState;

    static const int DECK_SIZE = 52;
    vector<Card> deck;

    vector<Card> currentHand;

    int numberOfDeals;
    std::unordered_set<int> seenCards;

    // Player initially always hits when they choose stand
    // Their choice is locked until the end of the episode
    bool playerChoseToHit = true;
    int dealerTotal = 0, playerTotal = 0;

public:
    Environment() {

        numberOfDeals = 0;

        // Defines the amount of space the deck will needed
        deck.reserve(60);
        deck.resize(52);

        for (int i = 0; i < DECK_SIZE; ++i) {
            // The minimum between 10 and the actual value is used because the maximum card value is 10 (excluding aces)
            CardVal currentCardVal = CardVal(std::min(10, 1 + i % 13));
            Suite currentSuite = Suite(POSSIBLE_SUITES[i / 13]);
            deck[i] = Card(i, currentCardVal, currentSuite);
            // cout << "Card " << i + 1 << " is -> " << deck[i] << "\n";
        }
        // cout << "\n";


        simulateNextRound(true);

        // currentHand = getNextHand();

        // cout << "Initial hand consists of " << currentHand.size() << " cards\n";
        // cout << "First card of initial hand is -> " << currentHand[0] << "\n";
        // cout << "Second card of initial hand is -> " << currentHand[1] << "\n";
        // cout << "\n";

        // for (int x : seenCards) {
        //     cout << "The " << x + 1 << "th card has been seen.\n";
        //     cout << "It was " << deck[x] << "\n";
        // }

    }

    /* Selects the index of an unseen card */
    int selectRandomIndex() {
        // Calculate the number of unseen cards
        int remainingCards = DECK_SIZE - (int)seenCards.size();
        // cout << "There are " <<  remainingCards << " cards currently available \n";

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
                // cout << "Skipping card " << i << " because it has been seen before\n";
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
        // cout << "Out of cards\n";
        return 0;
    }

    /* Generates the required number of cards for the current game state */
    vector<Card> getNextHand() {
        vector<Card> cardsDealt;
        vector<int> indicesChosen;

        // If this is the first deal then 4 cards are chosen; 2 for the dealer and 2 for the player
        // Otherwise 2 cards are chosen with one card for the dealer and one card for the player
        int numberOfCards;

        // Select 4 cards for the initial deal
        if (this->numberOfDeals == 0) {
            numberOfCards = 4;
        } else if (this->dealerTotal < 17 && this->playerChoseToHit) {
            // If the dealer can hit and the player chooses to hit then select 2 cards
            numberOfCards = 2;
        } else if (this->dealerTotal < 17 || this->playerChoseToHit) {
            // If either the dealer or player is capable then generate only one card
            numberOfCards = 1;
        } else {
            // If the dealer chooses to stand and the player stands then generate no more cards
            numberOfCards = 0;
        }

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

    void updateTotals(vector<Card> cardsDealt, int numberOfCards) {
        // If on the first hand or if all players choose to hit, then all sums are updated
        if ((this->numberOfDeals == 0) ||
            (this->dealerTotal < 17 && this->playerChoseToHit)) {
            for (int i = 0; i < numberOfCards; ++i) {
                // The dealer takes the cards on odd turns
                if (i % 2) {
                    this->dealerTotal += (int)cardsDealt[i].getValue();
                } else {
                    this->playerTotal += (int)cardsDealt[i].getValue();
                }
                currentState.addCard(cardsDealt[i], i % 2 == 0);
            }
        } else if (this->dealerTotal < 17) {
            // When only the dealer is able to hit
            this->dealerTotal += (int)cardsDealt[0].getValue();
            currentState.addCard(cardsDealt[0], false);
        } else if (this->playerChoseToHit) {
            // When only the player is able to hit
            this->playerTotal += (int)cardsDealt[0].getValue();
            currentState.addCard(cardsDealt[0], true);
        } else {
            // Neither the dealer or player chose or were able to hit so nothing to update sums with
            return;
        }
    }

    GameResult checkGameResult() {
        // If the dealer and player can still play, then the game has not reached an end state
        if (this->playerChoseToHit && this->playerTotal <= 21 && this->dealerTotal < 17) {
            cout << "\nPlayer says hit, Player hasn't bust yet and Dealer below 17.\n\n";
            return GameResult::Unfinished;
        }

        // If the dealer and player goes bust
        if (this->dealerTotal > 21 && this->playerTotal > 21) {
            return GameResult::MutualBust;
        } else if (this->dealerTotal > 21) {
            // If the dealer busts then the player wins
            return GameResult::PlayerWin;
        } else if (this->playerTotal > 21) {
            // If the player busts then the dealer wins
            return GameResult::DealerWin;

            // If the player chose to hit or the dealer must still hit then carry on
        } else if (this->playerChoseToHit || this->dealerTotal < 17) {
            cout << "\nPlayer chose to hit (" << (this->playerChoseToHit ? "True" : "False") <<
                ") OR Dealer below 17(" << (this->dealerTotal < 17 ? "True" : "False") << "), so game continues \n\n";

            // cout << "PlayerTotal = " << playerTotal << "\n";
            // cout << "DealerTotal = " << dealerTotal << "\n";
            return GameResult::Unfinished;

            // Below here the player must have chosen to stand and the dealer can push no further
        } else if (this->playerTotal > this->dealerTotal) {
            return GameResult::PlayerWin;
        } else if (this->playerTotal < this->dealerTotal) {
            return GameResult::DealerWin;
        } else {
            // Neither dealer nor player busted and neither can make a further move
            return GameResult::Push;
        }
    }

    GameResult simulateNextRound(bool hit) {
        // Take whether the agent chooses to hit or stand as input
        this->playerChoseToHit &= hit;

        // Get the next hand
        vector<Card> cardsDealt = getNextHand();

        // Update the totals using the cards dealt
        updateTotals(cardsDealt, cardsDealt.size());

        // Determine the current game state and return it
        GameResult gameResult = checkGameResult();
        currentState.setOutcome(gameResult);

        cout << currentState;
        cout << "Current outcome: " << currentState.getOutcome() << "\n";

        return gameResult;
    }

    GameState getCurrentState() {
        return this->currentState;
    }
};

// The initial agent with a fixed policy
/* Stand on or above 18 with probability 80% and hit when below 18 on probability 80%*/
class PassiveAgent {
private:
    // Q, N and returnSums store functions that keep track of the current state 
    // int Q[30][30][2],
    //     N[30][30][2],
    //     returnSums[30][30][2];

    // , returnSums, Q;3
    // The state is represented by the cards currently in play by both us and the dealer
    // Holds current gameState and the action taken
    vector<std::pair<GameState, bool>> Q, N, returnSums;
    float discountFactor = 0.90f;
    bool hit = true;
public:
    PassiveAgent() {

    }

    virtual bool policy(GameState state) {
        // Once the agent chooses to stand it cannot choose otherwise until the game is over
        if (!hit) {
            return false;
        }

        float probability = getRandomFloat();
        cout << "\nProbability value is -> " << probability << "\n";
        // If player total is less than 18 then choose to hit with probability 80% 
        if (state.getPlayerTotal() < 18) {
            cout << "Player Total is under 18, agent is biased towards hitting\n";
            hit = (probability <= 0.80f);
        } else {
            cout << "Player Total is greater than or equal to 18, agent is biased towards standing\n";
            // If player total is greater than 18 then choose to hit with probability 20%
            hit = (probability > 0.80f);
        }

        return hit;
    }

    virtual void addDecision(GameState state, bool action) {
        auto it = std::find_if(N.begin(), N.end(),
            [&](std::pair<GameState, bool> p) {
                return p.first == state && p.second == action;
            }
        );

        // If duplicate not found then add to vector
        if (it == N.end()) {
            cout << "The state-action pair has not been seen before so the agent stores it\n";
            N.emplace_back(state, action);
        } else {
            cout << "The state-action pair has not been seen before so the agent stores it\n";
        }
    }

    virtual bool considerNextState(GameState state) {
        bool action = policy(state);

        addDecision(state, action);

        return action;
    }

    vector<std::pair<GameState, bool>> getVisitCounts() {
        return N;
    }

    vector<std::pair<GameState, bool>> getReturnSums() {
        return returnSums;
    }

    /* The agent resets its choice */
    virtual void reset() {
        this->hit = true;
    }

};

int main() {
    srand(time(0));
    cout << "Enter the number of simulations: ";
    int numberOfSimulations;
    std::cin >> numberOfSimulations;
    cout << "\n";

    PassiveAgent agent;

    bool agentDecision;

    int statesSeen = 0;
    for (int i = 1; i <= numberOfSimulations; ++i) {
        cout << "SIMULATION " << i << ":\n";
        Environment testEnvironment;
        GameState state = testEnvironment.getCurrentState();
        agent.reset();
        // cout << testEnvironment.getCurrentState().getNumberOfSeenCards() << " cards seen.\n";

        while (state.getOutcome() == GameResult::Unfinished) {

            agentDecision = agent.considerNextState(state);
            ++statesSeen;
            cout << "\n" << (agentDecision ? "The agent chooses to hit." : "The agent chooses to stand.") << "\n";

            cout << "Now sleeping for 10 seconds \n";
            std::this_thread::sleep_for(milliseconds(10000));

            testEnvironment.simulateNextRound(agentDecision);
            state = testEnvironment.getCurrentState();
        }
        // Output the final game outcome
        cout << "Ultimate outcome: " << state.getOutcome() << "\n";
    }

    // NOTE: Need to implement hash function to store states-action pairs with their results and the returns_sums
    cout << "\nNow printing agent's stored states\n=====================================\n\n";
    vector<std::pair<GameState, bool>> visitCounts = agent.getVisitCounts();

    for (auto& p : visitCounts) {
        cout << p.first << (p.second ? "HIT" : "STAND") << "\n=====================================\n\n";
    }
    cout << "Number of counted states = " << statesSeen << " VS Number of states stored = " << visitCounts.size() << "\n";

    return 0;
}