#include <thread>
#include <chrono>

#include "agents.hpp"
#include "function.hpp"

using std::cout;
using std::cin;
using std::vector;
using StateAndAction = std::pair<environment::GameState, environment::Action>;

using namespace std::chrono;
// using namespace environment;

void updateQValues( function::StateActionFunction &Q,
                    function::StateActionFunction &N, 
                    function::StateActionFunction &returnSums){
        
    float *visitCountPtr = nullptr,
          *QValuePtr = nullptr,
          *returnSumPtr = nullptr;

    auto pointersAreValid = [&](){
        return  (
            visitCountPtr != nullptr && 
            QValuePtr != nullptr &&
            returnSumPtr != nullptr
        );
    };

    for (int i = 12; i <= environment::MAX_PLAYER_TOTAL; ++i){
        for (int j = 1; j <= environment::MAX_DEALER_SHOWING; ++j){
            for (int k = 0; k < environment::MAX_POSSIBLE_ACTIONS; ++k){

                visitCountPtr = N.getImage(i, j, k), 
                returnSumPtr = returnSums.getImage(i, j, k),
                QValuePtr = Q.getImage(i, j, k);

                // If the pointers are valid and the count is none zero
                // Then we set the QValue to be the average returnSum over all visits
                if (pointersAreValid() && (*visitCountPtr) > 0){
                    *QValuePtr = (*returnSumPtr) / (*visitCountPtr);
                    cout << (k ? "h":"s")<< " -> (S = {p: " << i << ", d: " << j << "}; Q = " << *QValuePtr << ") ";
                }

            }
        }
        cout << "\n";
    }
}

/* Extracts the game outcome and determines the reward value */
float generateRewardValue(environment::GameResult outcome){
    if (outcome == environment::GameResult::PLAYER_WIN){ // Win
        return 1.0f;
    } else if (outcome == environment::GameResult::DEALER_WIN){ // Loss
        return -1.0f;
    } else { // Draw
        return 0.0f;
    }
}


/* 
 Takes a state and  whether it was visited with an action previously and determines whether it should be recorded.
 States (and their related actions therein) should only be recorded if:
    * The state and action pair has not been visited previously
    * The player total is greater than or equal to 12 as a score under 12 is impossible to go bust on,
        so no agent decision is necessary,
    * The dealer is only showing one card, as the agent can make no further decisions after the dealer starts to
        reveal their hand,
 */
bool stateAndActionShouldBeRecorded(bool visited, environment::GameState state){
    return (
        !visited && 
        !state.allCardsFaceup() && 
        state.getPlayerTotal() >= 12    
    );
}

int main() {
    srand(time(0));

    function::StateActionFunction Q, N, returnSums, stateActionVisited;
    
    /* gamma is the discount factor that reduces the value of a reward over time */
    /* alpha is the learning factor that allows the agent to develop its own policy over time */
    float gamma = 0.90f, alpha;

    cout << "Enter the number of simulations: ";
    int numberOfSimulations;
    cin >> numberOfSimulations;
    cout << "\n";

    // Disable output
    cout.setstate(std::ios_base::failbit);

    // Clips the number of simulations to be between one and twenty
    numberOfSimulations = std::max(1, std::min(numberOfSimulations, 500000));

    // Initialise the passive agent
    agents::PassiveAgent agent;

    environment::Action agentDecision;
    
    // Stores the end result of each episode
    environment::GameResult gameOutcome;

    vector<StateAndAction> visitedStatesAndActions;

    int statesSeen = 0;

    auto start = high_resolution_clock::now();

    for (int i = 1; i <= numberOfSimulations; ++i) {
        cout << "SIMULATION #" << i << ":\n";
        environment::EnvironmentHandler testEnvironment;
        environment::GameState state = testEnvironment.getCurrentState();

        agent.reset();
        // cout << testEnvironment.getCurrentState().getNumberOfSeenCards() << " cards seen.\n";

        while (state.getOutcome() == environment::GameResult::UNFINISHED) {
            // Consider the state and return the decision made
            agentDecision = agent.considerState(state);
            ++statesSeen;
            cout << "\n";

            if (agentDecision == environment::Action::HIT){
                cout << "The agent chooses to hit.\n";
            } else {
                cout << "The agent chooses to stand.\n";
            }

            // Check whether it's the first visit and necessary to record
            // (IFF one dealer card is face up and agent action is non obvious)
            if ( stateActionVisited(state, agentDecision) != nullptr &&
                 stateAndActionShouldBeRecorded(
                    *stateActionVisited(state, agentDecision), state )   ){
                
                cout << "State not visited before" << "\n";
                cout << "(N) Count before incrementing = " << *N(state, agentDecision) << "\n"; 
                ++(*N(state, agentDecision));
                cout << "(N) Count after incrementing = " << *N(state, agentDecision) << "\n"; 
                ++(*stateActionVisited(state, agentDecision));

                visitedStatesAndActions.emplace_back(state, agentDecision);

            } else {
                cout << "Visited before :\n";
                cout << state << "\n";
            }

            testEnvironment.simulateNextRound(agentDecision);
            state = testEnvironment.getCurrentState();

            if (i % 5000 == 0){
                auto timeLog = high_resolution_clock::now();
                auto duration = duration_cast<milliseconds>(timeLog - start);

                std::clog << i << " simulations completed in " << duration.count() << "milliseconds\n\n";
            }
        }

        // G holds the reward of the current episode, 1 for win, 0 for draw, 1 for loss based on the game outcome
        float G = generateRewardValue( state.getOutcome() );

        // Iterate through the visitedStatesAndActions to update the returnSums now the reward is calculated
        while (!visitedStatesAndActions.empty()){
            StateAndAction current = visitedStatesAndActions.back();

            cout << "Return Sum before update = " << *returnSums(current.first, current.second) << "\n"; 

            // Updates the return sum with the value of the current reward
            *(returnSums(current.first, current.second)) += G;

            cout << "Return Sum after update = " << *returnSums(current.first, current.second)<< "\n"; 

            // Resets whether the state action has been visited for the generation of the next episode
            *(stateActionVisited(current.first, current.second)) = 0;

            visitedStatesAndActions.pop_back();
        }
        // Output the final game outcome
        cout << "Ultimate outcome: " << state.getOutcome() << "\n";

        // Resets all function outputs to zero, for the next episode
        // stateActionVisited->initialiseImages();

        cout << "Now sleeping for 5 seconds \n";
        // std::this_thread::sleep_for(milliseconds(5000));        
    }

    cout.clear();
    /* Prints the visit counts for each state action pair */
    cout << "Now printing State-Action visit counts\n"; 
    cout << N << "\n";

    cout << "Now printing the calculated return sums for each state and action\n";
    /* Prints the return sums for each state action pair */
    cout << returnSums << "\n";

    // Update q vales below here
    cout << "Now updating Q-Values \n";
    updateQValues(Q, N, returnSums);

    // vector<std::pair<environment::GameState, environment::Action>> visitCounts = agent.getVisitCounts();

    // for (auto& p : visitCounts) {
    //     cout << p.first << p.second << "=====================================\n\n";
    // }
    // cout << "Number of states stored = " << visitCounts.size() << "\n";

    return 0;
}