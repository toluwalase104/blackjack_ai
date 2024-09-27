#include <algorithm>

#include "agents.hpp"

using std::cout;
using std::vector;

// using namespace environment;
// using namespace agents;

agents::Agent::Agent(){
    cout << "Base constructor invoked Resetting action to hit in abstract base class\n\n";
    this->action = environment::Action::HIT;
    cout << "Current action is " << this->action << "\n";
}

agents::PassiveAgent::PassiveAgent() {}

/* Passive agent performs no actions other than applying the fixed-policy to a given state */
environment::Action agents::PassiveAgent::considerState(environment::GameState state){
    // Once the agent chooses to stand it cannot choose otherwise until the game is over
    if (action == environment::Action::STAND) {
        return environment::Action::STAND;
    }

    // If the sum is less than 12 then the agent will always hit since it is impossible to bust
    if (state.getPlayerTotal() < 12) {
        return environment::Action::HIT;
    }   

    return this->policy(state);
}

/* Enacts the agents policy depending on a given state */ 
environment::Action agents::PassiveAgent::policy(environment::GameState state) {
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

agents::GreedyAgent::GreedyAgent() : GreedyAgent(1.0f, 0.999f){}

agents::GreedyAgent::GreedyAgent(
    float epsilon, 
    float decayRate
): epsilon(epsilon), decayRate(decayRate), hitValue(0.0), standValue(0.0){
    this->action = environment::Action::HIT;
}


/*  Apply the policy to the given state.

    Decrease the size of epsilon after each decision, so as the agent handles more 
    simulations the probability of the agent choosing randomly decreases, exponentially.

    Starting with epsilon = 1 and a decay rate of 0.999, epsilon will equal EPSILON_MIN (0.01) after
    considering ~5000 states.
*/
environment::Action agents::GreedyAgent::considerState(environment::GameState state){
    // Once the agent chooses to stand it cannot choose otherwise until the game is over
    if (action == environment::Action::STAND) {
        return environment::Action::STAND;
    }

    // If the sum is less than 12 then the agent will always hit since it is impossible to bust
    if (state.getPlayerTotal() < 12) {
        return environment::Action::HIT;
    }
    // The policy is only applied and the exploration factor is only updated if there is an actual decision to be made

    environment::Action nextAction = this->policy(state);

    epsilon = std::max(epsilon * decayRate, EPSILON_MIN);

    return nextAction;
}

void agents::GreedyAgent::setActionValues(float hitValue, float standValue){
    this->hitValue = hitValue;
    this->standValue = standValue;
}

/*  The policy function uses epsilon-greedy to determine whether to explore or exploit
    upond receiving any given state.

    For each input state, it can choose to:

    * Use its past knowledge of the best action 
    (exploitation) 
    * Choose an action at random to "get a better idea" of how to handle the state in the future 
    (exploration)
    
    Initially when epsilon = 1, the probability of choosing best is 0.5 (completely random)
        i.e. = (1 - 1) + 1 / 2 = 0 + 1/2 = 1/2

    As epsilon tends towards 0, the probability of choosing the best action tends to 1 (stops at 0.995).
*/
environment::Action agents::GreedyAgent::policy(environment::GameState state){
    // The probability of choosing the best action tends towards 1 as the number of states seen increases
    float probabilityOfChoosingBest = (1.0 - epsilon) + epsilon / environment::MAX_POSSIBLE_ACTIONS;

    /* The probability of exploring is chosen at random */
    float probabilityOfExploring = environment::getRandomFloat();

    // By default set the agent's chosen action to whatever is most profitable
    if (hitValue > standValue){
        this->action = environment::Action::HIT;
    } else {
        this->action = environment::Action::STAND;
    }
    
    // If the policy is to choose the best then we return the best action
    if (probabilityOfChoosingBest > probabilityOfExploring){
        return this->action;
    }

    // If we reach here then the policy is to explore so we choose the opposite of the optimally selected action previously
    if (this->action == environment::Action::HIT){
        // If the best action was hit then we stand
        this->action = environment::Action::STAND;
    } else {
        // If the best thing to do was stand then we hit
        this->action = environment::Action::HIT;
    }
    
    return this->action;
}