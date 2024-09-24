#include <thread>
#include <chrono>

#include "agents.hpp"
#include "function.hpp"

/* Just experimenting with macros for the enums */
#define HIT environment::Action::HIT
#define STAND environment::Action::STAND

using std::cout;
using std::cin;
using std::vector;
using namespace std::chrono;

/* Stores a snapshot of the game's state alongside the action taken while in it*/ 
using StateAndAction = std::pair<environment::GameState, environment::Action>;

using namespace std::chrono;
// using namespace environment;

void updateQValues(
     function::StateActionFunction &Q,
    function::StateActionFunction &N, 
    function::StateActionFunction &returnSums
);

/* Q-Value update function for control function */
void updateQValues(
    function::StateActionFunction &Q,
    function::StateActionFunction &stateActionVisited,     
    std::vector<StateAndAction> &visitedStatesAndActions,
    int G,
    float learningFactor = 1.0
);

/* Extracts the game outcome and determines the reward value */
float generateRewardValue(environment::GameResult outcome);

/* 
 Takes a state and  whether it was visited with an action previously and determines whether it should be recorded.
 States (and their related actions therein) should only be recorded if:
    * The state and action pair has not been visited previously
    * The player total is greater than or equal to 12 as a score under 12 is impossible to go bust on,
        so no agent decision is necessary,
    * The dealer is only showing one card, as the agent can make no further decisions after the dealer starts to
        reveal their hand,
 */
bool stateAndActionShouldBeRecorded(bool visited, environment::GameState state);

void monteCarloPredict(
    int numberOfSimulations, 
    agents::PassiveAgent &agent, 
    std::vector<StateAndAction> &visitedStatesAndActions, 
    function::StateActionFunction &stateActionVisited, 
    function::StateActionFunction &N, 
    function::StateActionFunction &returnSums,
    std::chrono::_V2::system_clock::time_point &start
);

void monteCarloControl(
    int numberOfSimulations, 
    agents::GreedyAgent &agent, 
    function::StateActionFunction &Q, // Stores the utility for each of the state-action pairs 
    function::StateActionFunction &stateActionVisited, // Stores a 1 or a 0 if a state has or hasn't been visited
    std::vector<StateAndAction> &visitedStatesAndActions, 
    std::chrono::_V2::system_clock::time_point &start
);

void runEpisode(
    agents::PassiveAgent &agent, 
    environment::GameState &state, 
    environment::EnvironmentHandler &testEnvironment,    
    std::vector<StateAndAction> &visitedStatesAndActions, 
    function::StateActionFunction &stateActionVisited, 
    function::StateActionFunction &N
);

void updateReturnSums(
    environment::GameState &state, 
    std::vector<StateAndAction> &visitedStatesAndActions, 
    function::StateActionFunction &returnSums, 
    function::StateActionFunction &stateActionVisited
);

void outputValueFunction(
    function::StateActionFunction &Q
);

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

    // Clips the number of simulations to be between one and 500000
    numberOfSimulations = std::max(1, std::min(numberOfSimulations, 500000));

    // Initialise the passive agent
    // agents::PassiveAgent agent;

    // Initialise the greedy agent with epsilon = 1 and decay rate of 0.999
    agents::GreedyAgent agent(1, 0.999);
    
    vector<StateAndAction> visitedStatesAndActions;


    auto start = high_resolution_clock::now();

    // monteCarloPredict(numberOfSimulations, agent, visitedStatesAndActions, stateActionVisited, N, returnSums, start);
    monteCarloControl(numberOfSimulations, agent, Q, stateActionVisited, visitedStatesAndActions, start);


    /* Re-enables output */ 
    cout.clear();

    cout << "Now outputting max utility of each state\n\n";

    outputValueFunction(Q);

    cout << "\n\nNow outputting the value of Q\n";
    cout << Q << "\n";

    // /* Prints the visit counts for each state action pair */
    // cout << "Now printing State-Action visit counts\n"; 
    // cout << N << "\n";

    // cout << "Now printing the calculated return sums for each state and action\n";
    // /* Prints the return sums for each state action pair */
    // cout << returnSums << "\n";

    // // Update q vales below here
    // cout << "Now updating Q-Values \n";
    // updateQValues(Q, N, returnSums);
    // cout << "\n\n";
    // cout << Q << "\n";

    return 0;
}

void updateQValues( function::StateActionFunction &Q,
                    function::StateActionFunction &N, 
                    function::StateActionFunction &returnSums)
    {
        
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

float generateRewardValue(environment::GameResult outcome){
    if (outcome == environment::GameResult::PLAYER_WIN){ // Win
        return 1.0f;
    } else if (outcome == environment::GameResult::DEALER_WIN){ // Loss
        return -1.0f;
    } else { // Draw
        return 0.0f;
    }
}

bool stateAndActionShouldBeRecorded(bool visited, environment::GameState state){
    return (
        !visited && 
        !state.allCardsFaceup() && 
        state.getPlayerTotal() >= 12    
    );
}

void monteCarloPredict(
    int numberOfSimulations, 
    agents::PassiveAgent &agent, 
    std::vector<StateAndAction> &visitedStatesAndActions, 
    function::StateActionFunction &stateActionVisited, 
    function::StateActionFunction &N, 
    function::StateActionFunction &returnSums,
    std::chrono::_V2::system_clock::time_point &start
) {
    for (int i = 1; i <= numberOfSimulations; ++i){
        cout << "SIMULATION #" << i << ":\n";
        environment::EnvironmentHandler testEnvironment;
        environment::GameState state = testEnvironment.getCurrentState();

        agent.reset();

        /* Shift this to environment .hpp and .cpp then use the members of the class 
        to perform these actions internally */
        runEpisode(agent, state, testEnvironment, visitedStatesAndActions, stateActionVisited, N);

        updateReturnSums(state, visitedStatesAndActions, stateActionVisited, returnSums);

        // Output the final game outcome
        cout << "Ultimate outcome: " << state.getOutcome() << "\n";

        cout << "Now sleeping for 5 seconds \n";
        // std::this_thread::sleep_for(milliseconds(5000));
        if (i % 5000 == 0) {
            auto timeLog = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(timeLog - start);

            std::clog << i << " simulations completed in " << duration.count() << "milliseconds\n\n";
        }
    }
}

void monteCarloControl(
    int numberOfSimulations, 
    agents::GreedyAgent &agent, 
    function::StateActionFunction &Q, // Stores the utility for each of the state-action pairs 
    function::StateActionFunction &stateActionVisited, // Stores a 1 or a 0 if a state has or hasn't been visited
    std::vector<StateAndAction> &visitedStatesAndActions, 
    std::chrono::_V2::system_clock::time_point &start
) {
    for (int i = 1; i <= numberOfSimulations; ++i){
        cout << "SIMULATION #" << i << ":\n";
        environment::EnvironmentHandler testEnvironment;
        environment::GameState state = testEnvironment.getCurrentState();

        agent.reset();
        
        environment::Action agentDecision;

        /* Policy control starts here */
        while (state.getOutcome() == environment::GameResult::UNFINISHED){
            // Check if the state is valid before allowing the agent to make a decision modifying itself in the process
            // E.g. neither references returned from the function object should be null-pointers
            if (Q(state, HIT) != nullptr && Q(state, STAND) != nullptr){
                // Tell the agent what the optimal values are for hitting and standing given all prior states
                agent.setActionValues(*Q(state, HIT), *Q(state, STAND));
                // Consider the state and determine a decision to make
                agentDecision = agent.considerState(state);

                /* If first visit */
                if (stateActionVisited(state, agentDecision) != nullptr &&
                    stateAndActionShouldBeRecorded(
                        *stateActionVisited(state, agentDecision), state
                    )
                ){
                    ++(*stateActionVisited(state, agentDecision));
                    visitedStatesAndActions.emplace_back(state, agentDecision);
                }
            } else {
                 // The only time it would be a nullptr is if the agent chose to stand previously
                agentDecision = STAND;
            }

            testEnvironment.simulateNextRound(agentDecision);
            state = testEnvironment.getCurrentState();
        }

        // Generate the reward value from the result of the game
        int reward = generateRewardValue( state.getOutcome() );

        updateQValues(Q, stateActionVisited, visitedStatesAndActions, reward, 0.001f);

        cout << "Now sleeping for 5 seconds \n";
        // std::this_thread::sleep_for(milliseconds(5000));
        if (i % 5000 == 0) {
            auto timeLog = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(timeLog - start);

            std::clog << i << " simulations completed in " << duration.count() << "milliseconds\n\n";
        }
    }
}

void updateQValues(
    function::StateActionFunction &Q,
    function::StateActionFunction &stateActionVisited,     
    std::vector<StateAndAction> &visitedStatesAndActions,
    int G,
    float learningFactor
){
    for (StateAndAction &p: visitedStatesAndActions){
        environment::GameState state = p.first;
        environment::Action action = p.second;
        /* Calculate the updates to the Q-Value using the learning factor to prevent rapid and drastic changes */
        *Q(state, action) = *Q(state, action) + learningFactor * (G - *Q(state, action));

        // Set the state and action to be unvisited for future calculations
        *stateActionVisited(state, action) = 0;
    }
    // Clear the vector for future calculations
    visitedStatesAndActions.clear();
}

void runEpisode(
    agents::PassiveAgent &agent, 
    environment::GameState &state, 
    environment::EnvironmentHandler &testEnvironment,    
    std::vector<StateAndAction> &visitedStatesAndActions, 
    function::StateActionFunction &stateActionVisited, 
    function::StateActionFunction &N
) {
    while (state.getOutcome() == environment::GameResult::UNFINISHED) {
        // Consider the state and return the decision made
        environment::Action agentDecision = agent.considerState(state);
        cout << "\n";

        if (agentDecision == HIT) {
            cout << "The agent chooses to hit.\n";
        } else {
            cout << "The agent chooses to stand.\n";
        }

        // Check whether it's the first visit and necessary to record
        // (IFF one dealer card is face up and agent action is non obvious)
        if (stateActionVisited(state, agentDecision) != nullptr &&
            stateAndActionShouldBeRecorded(
                *stateActionVisited(state, agentDecision), state
            )
        ){

            // cout << "State not visited before" << "\n";
            // cout << "(N) Count before incrementing = " << *N(state, agentDecision) << "\n";

            ++(*N(state, agentDecision));

            // cout << "(N) Count after incrementing = " << *N(state, agentDecision) << "\n";

            ++(*stateActionVisited(state, agentDecision));

            visitedStatesAndActions.emplace_back(state, agentDecision);
        } else {
            cout << "Visited before :\n";
            cout << state << "\n";
        }

        testEnvironment.simulateNextRound(agentDecision);
        state = testEnvironment.getCurrentState();
    }
}

void updateReturnSums(
    environment::GameState &state, 
    std::vector<StateAndAction> &visitedStatesAndActions, 
    function::StateActionFunction &stateActionVisited, 
    function::StateActionFunction &returnSums 
) {
    // G holds the reward of the current episode, 1 for win, 0 for draw, 1 for loss based on the game outcome
    float G = generateRewardValue(state.getOutcome());

    // Iterate through the visitedStatesAndActions to update the returnSums now the reward is calculated
    while (!visitedStatesAndActions.empty()) {
        StateAndAction current = visitedStatesAndActions.back();

        cout << "Return Sum before update = " << *returnSums(current.first, current.second) << "\n";

        // Updates the return sum with the value of the current reward
        *(returnSums(current.first, current.second)) += G;

        cout << "Return Sum after update = " << *returnSums(current.first, current.second) << "\n";

        // Resets whether the state action has been visited for the generation of the next episode
        *(stateActionVisited(current.first, current.second)) = 0;

        visitedStatesAndActions.pop_back();
    }
}

void outputValueFunction(
    function::StateActionFunction &Q
){
    /*  Outputs the value function by combining the q-values for each action in a given state
        into a sum for the entire state, since the q-values are averages */
    for (int i = 12; i <= environment::MAX_PLAYER_TOTAL; ++i){
        for (int j = 1; j <= environment::MAX_DEALER_SHOWING; ++j){
            // Get the max value for a given state
            float value = std::max(*Q.getImage(i, j, 0), *Q.getImage(i, j, 1));
            
            cout << "(p = " << i << "; d = " << j << "; value = " << value << ") ";
        }
        cout << "\n";
    }
}
