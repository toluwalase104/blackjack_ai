#include "agents.hpp"

using std::cout;
using std::vector;

// using namespace environment;
// using namespace agents;

agents::PassiveAgent::PassiveAgent() {
    cout << "Setting discount factor and hit value\n";
    discountFactor = 0.90f;
    action = environment::Action::Hit;
}

/* Enacts the agents policy depending on a given state */ 
environment::Action agents::PassiveAgent::policy(environment::GameState state) {
    // Once the agent chooses to stand it cannot choose otherwise until the game is over
    if (action == environment::Action::Stand) {
        return environment::Action::Stand;
    }

    // If the sum is less than 12 then the agent will always hit since it is impossible to bust
    if (state.getPlayerTotal() < 12) {
        return environment::Action::Hit;
    }   

    float probability = environment::getRandomFloat();
    cout << "\nProbability value is -> " << probability << "\n";
    // If player total is less than 18 then choose to hit with probability 80% 
    if (state.getPlayerTotal() < 18) {
        cout << "Player Total is under 18, agent is biased towards hitting\n";
        action = environment::Action(probability <= 0.80f);
    } else {
        cout << "Player Total is greater than or equal to 18, agent is biased towards standing\n";
        // If player total is greater than 18 then choose to hit with probability 20%
        action = environment::Action(probability > 0.80f);
    }

    return action;
}


/* Considers a given state and returns the action decided by the agent in that state */
environment::Action agents::PassiveAgent::considerState(environment::GameState state) {
    environment::Action action = policy(state);

    return action;
}

/* The agent resets its choice */
void agents::PassiveAgent::reset() {
    this->action = environment::Action::Hit;
}