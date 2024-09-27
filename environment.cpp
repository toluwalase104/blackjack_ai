#include "environment.hpp"

using std::cout;
using std::vector;
// using namespace environment;
// using namespace game_assets;

environment::GameState::GameState() :
    playerTotal(0), dealerTotal(0), faceupTotal(0),
    numberOfDeals(0), numberOfSeenCards(0),
    dealerShowsAll(false),
    playerHasUsableAce(false),
    dealerHasUsableAce(false),
    outcome(GameResult::UNFINISHED) {
        playerCards.reserve(60), dealerCards.reserve(60);
        playerCards.resize(52, 0), dealerCards.resize(52, 0);
}

bool environment::GameState::doesPlayerHaveUsableAce() const {
    return playerHasUsableAce;
}

bool environment::GameState::doesDealerHaveUsableAce() const {
    return dealerHasUsableAce;
}

void environment::GameState::updateTotal(
    int cardValue,
    int &total,
    bool &usableAce
){
    // If the player has a usable ace and they are about to go bust then deactivate the ace
    if (usableAce && total + cardValue > 21){
        usableAce = false;
        total += cardValue - 10;
        return;
    }
    // If the player is about to receive an ace
    if (cardValue == 1 && total + 11 <= 21){
        usableAce = true;
        total += 11;
        return;
    }

    // Otherwise increment the total as normal
    total += cardValue;
}

void environment::GameState::addCard(game_assets::Card card, bool forPlayer) {
    int cardID = card.getID();

    // If seen the card was seen do not add it again
    if (cardSeen(cardID)) {
        return;
    }

    int cardValue = (int)card.getValue();

    // Give card to the appropriate owner
    if (forPlayer) {
        playerCards[cardID] = 1;

        updateTotal(cardValue, playerTotal, playerHasUsableAce);
    } else {
        dealerCards[cardID] = 1;

        updateTotal(cardValue, dealerTotal, dealerHasUsableAce);

        // Modify the face up total if need be 
        faceupTotal = 
            (faceupTotal == 0 || dealerShowsAll) 
            ? dealerTotal
            : faceupTotal;
    }

    ++numberOfSeenCards;
}

int environment::GameState::getPlayerTotal() const{
    return playerTotal;
}

int environment::GameState::getDealerTotal() const{
    return dealerTotal;
}

int environment::GameState::getFaceupTotal() const{
    return faceupTotal;
}

/* Returns all cards seen so far, combine into one set */
std::vector<int> environment::GameState::getSeenCards() const{
    vector<int> result;

    // Reserve enough space for both sets of cards
    result.reserve(numberOfSeenCards); 

    std::copy_if(playerCards.begin(), playerCards.end(), std::back_inserter(result),
        [](int x){
            return x;
        }
    );

    std::copy_if(dealerCards.begin(), dealerCards.end(), std::back_inserter(result),
        [](int x){
            return x;
        }
    );

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
    return playerCards[cardID] || dealerCards[cardID];
}

bool environment::GameState::cardSeen(int cardID) {
    return playerCards[cardID] || dealerCards[cardID];
}

int environment::GameState::getNumberOfSeenCards() const{
    return numberOfSeenCards;
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
    
    for (int i = 0; i < environment::DECK_SIZE; ++i) {
        // Output only the seen cards
        if (playerCards[i]){
            playerCardStrings += "    " + convertFaceValue(i) + convertSuite(i) + "\n";
        }
    }
    playerCardStrings += "  }\n";

    for (int i = 0; i < environment::DECK_SIZE; ++i) {
        // Output only the seen cards
        if (dealerCards[i]){
            dealerCardStrings += "    " + convertFaceValue(i) + convertSuite(i) + "\n";
        }
    }
    dealerCardStrings += "  }\n";

    return playerCardStrings + dealerCardStrings;
}

/* Makes the dealer show all the cards */
void environment::GameState::showDealerCards(){
    this->dealerShowsAll = true;
}

/* Shows if dealer is showing all the cards */ 
bool environment::GameState::dealerCardsShown(){
    return this->dealerShowsAll;
}

/* State equality is determined by the player and dealer sum for simplicity */
bool environment::GameState::operator==(GameState comparedState) {
    return (
        (this->getPlayerTotal() == comparedState.getPlayerTotal()) &&
        (this->getDealerTotal() == comparedState.getDealerTotal())
    );
}

environment::EnvironmentHandler::EnvironmentHandler() {
    // Defines the amount of space the deck will needed
    deck.reserve(60);
    deck.resize(52);
    
    numberOfRemainingCards = DECK_SIZE;

    for (int i = 0; i < DECK_SIZE; ++i) {
        // The minimum between 10 and the actual value is used because the maximum card value is 10 (excluding aces)
        game_assets::CardVal currentCardVal = game_assets::CardVal(std::min(10, 1 + i % 13));
        game_assets::Suite currentSuite = game_assets::Suite(POSSIBLE_SUITES[i / 13]);
        deck[i] = game_assets::Card(i, currentCardVal, currentSuite);
    }

    // Start the game with the initial player action being hit
    simulateNextRound(environment::Action::HIT);
}

/* Selects a card that has not been seen yet */
int environment::EnvironmentHandler::selectOutOfRemainingCards() {
    cout << "\nA card is being randomly selected out of the " << numberOfRemainingCards << " remaining.\n";
    // Choose an index out of the remaining cards to select
    int index = rand() % numberOfRemainingCards;

    // Utility function to output numbers in a correct human-readable format
    auto getNumberSuffix = [](int i){
        switch(i % 10){
            case 1:
                return "st";
            case 2:
                return "nd";
            case 3:
                return "rd";
            default:
                return "th";
        }
    };
    cout << "The " << index << getNumberSuffix(index) << " unseen card out of those remaining will be selected.\n";
    // Search for the chosen index
    cout << "Enterring selection loop\n";

    for (int i = 0; i < DECK_SIZE; ++i) {
        // If the current card has been seen skip it
        if (seenIDs.find(i) != seenIDs.end()) {
            continue;
        }

        if (index <= 0) {
            --numberOfRemainingCards;
            cout << "The " << i << getNumberSuffix(i) << "card in the deck was chosen";
            seenIDs.emplace(i);
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

    // Select 4 cards for the initial deal, 2 player cards one dealer card
    if (currentState.numberOfDeals == 0) {
        numberOfCards = 4;
    } else if (!currentState.dealerCardsShown() || currentState.getDealerTotal() < 17) { 
        // Generate one card for each deal after 
        numberOfCards = 1;
    } else {
        // If the dealer chooses to stand and the player stands then generate no more cards
        numberOfCards = 0;
    }

    // Select some number of cards randomly
    int currentIndex;
    for (int i = 0; i < numberOfCards; ++i) {
        currentIndex = selectOutOfRemainingCards();

        cardsDealt.emplace_back(deck[currentIndex]);
    }

    return cardsDealt;
}

void environment::EnvironmentHandler::updateTotals(vector<game_assets::Card> cardsDealt, int numberOfCards) {
    // If on the first hand then all sums are updated
    if (currentState.numberOfDeals == 0) {
        for (int i = 0; i < numberOfCards; ++i) {
            // The dealer takes the cards on odd turns and the player on even turns
            currentState.addCard(cardsDealt[i], i % 2 == 0);
        }
    // If the dealer still has a card facing down then the player is still hitting
    } else if (!currentState.dealerCardsShown()) { 
        // While the player chooses to hit, only the player will be served
        currentState.addCard(cardsDealt[0], true);
    } else if (currentState.getDealerTotal() < 17) {
        // When only the dealer is able to hit
        currentState.addCard(cardsDealt[0], false);
    } else {
        // Neither the dealer or player chose or were able to hit so nothing to update sums with
        return;
    }
}

environment::GameResult environment::EnvironmentHandler::checkGameResult() {
    // If the dealer and player can still play, then the game has not reached an end state
    if (!currentState.dealerCardsShown() && currentState.getPlayerTotal() <= 21 && currentState.getDealerTotal() < 17) {
        cout << "\nPlayer says hit, Player hasn't bust yet and Dealer below 17; so the game continues.\n\n";
        return environment::GameResult::UNFINISHED;
    }

    // If the dealer and player goes bust
    if (currentState.getDealerTotal() > 21 && currentState.getPlayerTotal() > 21) {
        return environment::GameResult::MUTUAL_BUST;
    } else if (currentState.getDealerTotal() > 21) {
        // If the dealer busts then the player wins
        return environment::GameResult::PLAYER_WIN;
    } else if (currentState.getPlayerTotal() > 21) {
        // If the player busts then the dealer wins
        return environment::GameResult::DEALER_WIN;

        // If the player chose to hit or the dealer must still hit then carry on
    } else if (!currentState.dealerCardsShown() || currentState.getDealerTotal() < 17) {
        cout << "\n" << "Player chose to hit (" << (!currentState.dealerCardsShown() ? "True" : "False") <<
            ") OR Dealer below 17(" << (currentState.getDealerTotal() < 17 ? "True" : "False") << "), so game continues \n\n";
            
        return environment::GameResult::UNFINISHED;

        // Below here the player must have chosen to stand and the dealer can push no further
    } else if (currentState.getPlayerTotal() > currentState.getDealerTotal()) {
        return environment::GameResult::PLAYER_WIN;
    } else if (currentState.getPlayerTotal() < currentState.getDealerTotal()) {
        return environment::GameResult::DEALER_WIN;
    } else {
        // Neither dealer nor player busted and neither can make a further move
        return environment::GameResult::PUSH;
    }
}

/*  The control function of the environment object.
    It plays the next hand of the game, updates the player totals and  determines whether the game is over.*/
environment::GameResult environment::EnvironmentHandler::simulateNextRound(environment::Action action) {
    // If the player ever chooses to stand, the dealer reveals the face down card
    if (action == environment::Action::STAND){
        currentState.showDealerCards();
    }

    // Get the next hand
    vector<game_assets::Card> cardsDealt = getNextHand();

    // Update the totals using the cards dealt
    updateTotals(cardsDealt, cardsDealt.size());

    // Determine the current game state and return it
    environment::GameResult gameResult = checkGameResult();

    currentState.setOutcome(gameResult);

    cout << currentState;
    cout << "Current outcome: " << currentState.getOutcome() << "\n";
    cout << "The PLAYER has a usable ace = " << (currentState.doesPlayerHaveUsableAce() ? "TRUE":"FALSE") << "\n";
    cout << "The DEALER has a usable ace = " << (currentState.doesDealerHaveUsableAce() ? "TRUE":"FALSE") << "\n\n";
    ++currentState.numberOfDeals;
    return gameResult;
}

environment::GameState environment::EnvironmentHandler::getCurrentState() const{
    return this->currentState;
}

std::ostream& operator<<(std::ostream& o, environment::GameState s) {
    cout << "GameState :{\n" <<
        "  Player Total = " << s.getPlayerTotal() << "\n" <<
        "  Dealer Total (currently shown) = " << s.getFaceupTotal() << "\n" << 
        s.stringifyCards() << "}\n\n";
    return o;
}

std::ostream& operator<<(std::ostream& o, environment::GameResult r) {
    switch (r) {
        case environment::GameResult::DEALER_WIN:
            cout << "The Dealer won\n";
            break;
        case environment::GameResult::MUTUAL_BUST:
            cout << "Player and Dealer busted\n";
            break;
        case environment::GameResult::UNFINISHED:
            cout << "The game is unfinished\n";
            break;
        case environment::GameResult::PUSH:
            cout << "Equal non-bust score\n";
            break;
        case environment::GameResult::PLAYER_WIN:
            cout << "The Player won\n";
            break;
        default:
            cout << "GameResult undefined\n";
    }
    return o;
}

std::ostream& operator<<(std::ostream& o, environment::Action a) {
    if (a == environment::Action::HIT){
        cout << "HIT" << "\n";
    } else {
        cout << "STAND" << "\n";
    }
    return o;
}