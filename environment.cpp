#include "environment.hpp"

using std::cout;
using std::vector;
// using namespace environment;
// using namespace game_assets;

environment::GameState::GameState() :
    playerTotal(0), dealerTotal(0), outcome(GameResult::Unfinished) {
}

void environment::GameState::addCard(game_assets::Card card, bool forPlayer) {
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

int environment::GameState::getPlayerTotal() const{
    return playerTotal;
}

int environment::GameState::getDealerTotal() const{
    return dealerTotal;
}

/* Returns all cards seen so far, combine into one set */
std::set<int> environment::GameState::getSeenCards() const{
    std::set<int> result(playerCards.begin(), playerCards.end());

    result.insert(dealerCards.begin(), dealerCards.end());

    return result;
}

void environment::GameState::setOutcome(GameResult outcome) {
    this->outcome = outcome;
}
environment::GameResult environment::GameState::getOutcome() const{
    return outcome;
}

bool environment::GameState::cardSeen(game_assets::Card card) {
    int cardID = card.getID();
    return (playerCards.find(cardID) != playerCards.end()) ||
        (dealerCards.find(cardID) != dealerCards.end());
}

bool environment::GameState::cardSeen(int cardID) {
    return (playerCards.find(cardID) != playerCards.end()) ||
        (dealerCards.find(cardID) != dealerCards.end());
}

int environment::GameState::getNumberOfSeenCards() const{
    return dealerCards.size() + playerCards.size();
}

/* Allows the pretty printing of currently stored cards */
std::string environment::GameState::stringifyCards() {
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
            return " game_assets::Suite was undefined ";
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

bool environment::GameState::operator==(GameState comparedState) {
    return this->getSeenCards() == comparedState.getSeenCards();
}

environment::EnvironmentHandler::EnvironmentHandler() {

    playerTotal = 0;
    dealerTotal = 0;
    numberOfDeals = 0;

    // Defines the amount of space the deck will needed
    deck.reserve(60);
    deck.resize(52);

    for (int i = 0; i < DECK_SIZE; ++i) {
        // The minimum between 10 and the actual value is used because the maximum card value is 10 (excluding aces)
        game_assets::CardVal currentCardVal = game_assets::CardVal(std::min(10, 1 + i % 13));
        game_assets::Suite currentSuite = game_assets::Suite(POSSIBLE_SUITES[i / 13]);
        deck[i] = game_assets::Card(i, currentCardVal, currentSuite);
    }

    simulateNextRound(true);
}

/* Selects the index of an unseen card */
int environment::EnvironmentHandler::selectRandomIndex() {
    // Calculate the number of unseen cards
    int remainingCards = DECK_SIZE - (int)seenCards.size();

    // Choose one of the cards from the unseen cards at random
    return (rand() % remainingCards) + 1;
}

/* Selects a card that has not been seen yet */
int environment::EnvironmentHandler::selectOutOfRemainingCards() {
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
    return 0;
}

/* Generates the required number of cards for the current game state */
vector<game_assets::Card> environment::EnvironmentHandler::getNextHand() {
    vector<game_assets::Card> cardsDealt;
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

void environment::EnvironmentHandler::updateTotals(vector<game_assets::Card> cardsDealt, int numberOfCards) {
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

environment::GameResult environment::EnvironmentHandler::checkGameResult() {
    // If the dealer and player can still play, then the game has not reached an end state
    if (this->playerChoseToHit && this->playerTotal <= 21 && this->dealerTotal < 17) {
        cout << "\nPlayer says hit, Player hasn't bust yet and Dealer below 17; so the game continues.\n\n";
        return environment::GameResult::Unfinished;
    }

    // If the dealer and player goes bust
    if (this->dealerTotal > 21 && this->playerTotal > 21) {
        return environment::GameResult::MutualBust;
    } else if (this->dealerTotal > 21) {
        // If the dealer busts then the player wins
        return environment::GameResult::PlayerWin;
    } else if (this->playerTotal > 21) {
        // If the player busts then the dealer wins
        return environment::GameResult::DealerWin;

        // If the player chose to hit or the dealer must still hit then carry on
    } else if (this->playerChoseToHit || this->dealerTotal < 17) {
        cout << "\n" << "Player chose to hit (" << (this->playerChoseToHit ? "True" : "False") <<
            ") OR Dealer below 17(" << (this->dealerTotal < 17 ? "True" : "False") << "), so game continues \n\n";
            
        return environment::GameResult::Unfinished;

        // Below here the player must have chosen to stand and the dealer can push no further
    } else if (this->playerTotal > this->dealerTotal) {
        return environment::GameResult::PlayerWin;
    } else if (this->playerTotal < this->dealerTotal) {
        return environment::GameResult::DealerWin;
    } else {
        // Neither dealer nor player busted and neither can make a further move
        return environment::GameResult::Push;
    }
}

environment::GameResult environment::EnvironmentHandler::simulateNextRound(bool hit) {
    // Take whether the agent chooses to hit or stand as input
    this->playerChoseToHit &= hit;

    // Get the next hand
    vector<game_assets::Card> cardsDealt = getNextHand();

    // Update the totals using the cards dealt
    updateTotals(cardsDealt, cardsDealt.size());

    // Determine the current game state and return it
 environment::GameResult gameResult = checkGameResult();
    currentState.setOutcome(gameResult);

    cout << currentState;
    cout << "Current outcome: " << currentState.getOutcome() << "\n";

    return gameResult;
}

environment::GameState environment::EnvironmentHandler::getCurrentState() const{
    return this->currentState;
}

std::ostream& operator<<(std::ostream& o, environment::GameState s) {
    cout << "GameState :{\n" <<
        "  Player Total = " << s.getPlayerTotal() << "\n" <<
        "  Dealer Total = " << s.getDealerTotal() << "\n" <<
        s.stringifyCards() << "}\n\n";
    return o;
}

std::ostream& operator<<(std::ostream& o, environment::GameResult r) {
    switch (r) {
    case environment::GameResult::DealerWin:
        cout << "The Dealer won\n";
        break;
    case environment::GameResult::MutualBust:
        cout << "Player and Dealer busted\n";
        break;
    case environment::GameResult::Unfinished:
        cout << "The game is unfinished\n";
        break;
    case environment::GameResult::Push:
        cout << "Equal non-bust score\n";
        break;
    case environment::GameResult::PlayerWin:
        cout << "The Player won\n";
        break;
    default:
        cout << "GameResult undefined\n";
    }
    return o;
}

