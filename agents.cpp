#include "agents.hpp"

using std::cout;
using std::vector;

// using namespace environment;
// using namespace agents;

agents::PassiveAgent::PassiveAgent() {
    cout << "Setting discount factor and hit value\n";
    discountFactor = 0.90f;
    hit = true;
}

bool agents::PassiveAgent::policy(environment::GameState state) {
    // Once the agent chooses to stand it cannot choose otherwise until the game is over
    if (!hit) {
        return false;
    }

    float probability = environment::getRandomFloat();
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

void agents::PassiveAgent::addDecision(environment::GameState state, bool action) {
    auto it = std::find_if(N.begin(), N.end(),
        [&](std::pair<environment::GameState, bool> p) {
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

bool agents::PassiveAgent::considerNextState(environment::GameState state) {
    bool action = policy(state);

    addDecision(state, action);

    return action;
}

vector<std::pair<environment::GameState, bool>> agents::PassiveAgent::getVisitCounts() {
    return N;
}

vector<std::pair<environment::GameState, bool>> agents::PassiveAgent::getReturnSums() {
    return returnSums;
}

/* The agent resets its choice */
void agents::PassiveAgent::reset() {
    this->hit = true;
}