#include <thread>
#include <chrono>

#include "agents.hpp"
#include "function.hpp"

/* Just experimenting with macros for the enums */
#define HIT environment::Action::HIT
#define STAND environment::Action::STAND
#define MAX_NUMBER_OF_SIMULATIONS 1000000
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
    std::vector<StateAndAction> &visitedStatesAndActions,
    float G,
    float learningFactor
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
bool stateAndActionShouldBeRecorded(environment::GameState state);

void monteCarloPredict(
    int numberOfSimulations, 
    agents::PassiveAgent &agent, 
    std::vector<StateAndAction> &visitedStatesAndActions, 
    function::StateActionFunction &N, 
    function::StateActionFunction &returnSums
);

void monteCarloControl(
    int numberOfSimulations, 
    agents::GreedyAgent &agent, 
    function::StateActionFunction &Q, // Stores the utility for each of the state-action pairs 
    std::vector<StateAndAction> &visitedStatesAndActions
);

void runEpisode(
    agents::PassiveAgent &agent, 
    environment::GameState &state, 
    environment::EnvironmentHandler &testEnvironment,    
    std::vector<StateAndAction> &visitedStatesAndActions, 
    function::StateActionFunction &N
);

void updateReturnSums(
    environment::GameState &state, 
    std::vector<StateAndAction> &visitedStatesAndActions, 
    function::StateActionFunction &returnSums
);

void outputValueFunction(
    function::StateActionFunction &Q
);

/* Stores the agents initial sum */
long long currentWinnings = 1000;
/* Stores the lowest winings ever received*/
long long highestWinnings = 0;
/* Stores the total rreward value across the entire simulation */
double cumulativeReward = 0.0;

int COUNT = 0;

std::string confirmation;

int main() {
    std::ios_base::sync_with_stdio(false);

    srand(time(0));

    function::StateActionFunction Q, N, returnSums;
    
    cout << "Enter the number of simulations: ";
    int numberOfSimulations;
    cin >> numberOfSimulations;
    cout << "\n";

    // Disable output
    cout.setstate(std::ios_base::failbit);

    // Clips the number of simulations to be between one and one million
    numberOfSimulations = std::max(1, std::min(numberOfSimulations, MAX_NUMBER_OF_SIMULATIONS));

    // Initialise the passive agent
    // agents::PassiveAgent agent;

    // Initialise the greedy agent with epsilon = 1 and decay rate of 0.999
    agents::GreedyAgent agent;
    
    vector<StateAndAction> visitedStatesAndActions;

    // monteCarloPredict(numberOfSimulations, agent, visitedStatesAndActions, stateActionVisited, N, returnSums);
    monteCarloControl(numberOfSimulations, agent, Q, visitedStatesAndActions);


    /* Re-enables output */ 
    cout.clear();

    cout << "Now outputting max utility of each state\n\n";

    outputValueFunction(Q);

    cout << "\n\nNow outputting the value of Q\n";
    cout << Q << "\n\n";

    cout << "Final winnings = " << currentWinnings << "\n";
    cout << "Highest winnings = " << highestWinnings << "\n";
    cout << "Expected reward = " << cumulativeReward / numberOfSimulations << "\n";
    cout << COUNT << " states were visited more than once.\n";

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

void updateQValues( 
    function::StateActionFunction &Q,
    function::StateActionFunction &N, 
    function::StateActionFunction &returnSums
){
        
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

    for (int l = 0; l < 2; ++l){
        cout << (l ? "":"NO") << "Usable Ace\n";
        for (int i = 12; i <= environment::MAX_PLAYER_TOTAL; ++i){
            for (int j = 2; j <= environment::MAX_DEALER_SHOWING; ++j){
                for (int k = 0; k < environment::MAX_POSSIBLE_ACTIONS; ++k){
                        visitCountPtr = N.getImage(i, j, k, l), 
                        returnSumPtr = returnSums.getImage(i, j, k, l),
                        QValuePtr = Q.getImage(i, j, k, l);

                        // If the pointers are valid and the count is none zero
                        // Then we set the QValue to be the average returnSum over all visits
                        if (pointersAreValid() && (*visitCountPtr) > 0){
                            *QValuePtr = (*returnSumPtr) / (*visitCountPtr);
                            cout << (k ? "h":"s")<< " -> (S = {p: " << i << ", d: " << j << "}; Q = " << *QValuePtr << "; A = " << l << ") ";
                        }

                }
            }
            cout << "\n";
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

bool stateAndActionShouldBeRecorded(environment::GameState state){
    cout << "Now checking whether to record the current state and action\n";
    cout << "The dealers face down cards are " << (state.dealerCardsShown() ? "":"not") << " shown.\n";
    cout << "The player total is " << 
        (state.getPlayerTotal() >= 12   ? "greater than or equal to 12":"less than 12") << ".\n";

    return (
        !state.dealerCardsShown() && 
        state.getPlayerTotal() >= 12    
    );
}

/*  Takes a passive agent with a fixed policy and 
    stores the Q-Values related with its decisions.
    */
void monteCarloPredict(
    int numberOfSimulations, 
    agents::PassiveAgent &agent, 
    std::vector<StateAndAction> &visitedStatesAndActions, 
    function::StateActionFunction &N, 
    function::StateActionFunction &returnSums
){
    auto start = high_resolution_clock::now();
    
    cout << "Now evaluating the results of a fixed policy with a passive agent.\n";
    for (int i = 1; i <= numberOfSimulations; ++i){
        cout << "SIMULATION #" << i << ":\n";
        environment::EnvironmentHandler testEnvironment;
        environment::GameState state = testEnvironment.getCurrentState();

        agent.reset();

        /* Shift this to environment .hpp and .cpp then use the members of the class 
        to perform these actions internally */
        runEpisode(agent, state, testEnvironment, visitedStatesAndActions, N);

        updateReturnSums(state, visitedStatesAndActions, returnSums);

        // Output the final game outcome
        cout << "Ultimate outcome: " << state.getOutcome() << "\n";

        // cout << "Now sleeping for 15 seconds \n";
        // std::this_thread::sleep_for(milliseconds(15000));

        std::string userInput("");
        do {
            cout << "Do you want to continue the simulation? (y/n)" << "\n";
            cin >> userInput;
            cout << "You enterred \"" << userInput << "\"\n";
        } while (userInput != "y" && userInput != "n" && userInput != "N" && userInput != "Y");

        if (userInput == "n" || userInput == "N"){
            break;
        }

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
    std::vector<StateAndAction> &visitedStatesAndActions
) {
    auto start = high_resolution_clock::now();

    for (int i = 1; i <= numberOfSimulations; ++i){
        cout << "SIMULATION #" << i << ":\n";
        environment::EnvironmentHandler testEnvironment;
        environment::GameState state = testEnvironment.getCurrentState();

        agent.reset();
        cout << "Initial agent action = " << agent.getAction() << "\n";

        environment::Action agentDecision = agent.getAction();

        /* Policy control starts here */
        /* For actual */
        while (state.getOutcome() == environment::GameResult::UNFINISHED){
            // Check if the state is valid before allowing the agent to make a decision modifying itself in the process
            // E.g. neither references returned from the function object should be null-pointers
            if (Q(state, HIT) != nullptr && Q(state, STAND) != nullptr){
                // Tell the agent what the optimal values are for hitting and standing given all prior states
                agent.setActionValues(*Q(state, HIT), *Q(state, STAND));

                // Consider the state and determine a decision to make
                agentDecision = agent.considerState(state);

                if (agentDecision == HIT) {
                    cout << "The agent chooses to hit.\n";
                } else {
                    cout << "The agent chooses to stand.\n";
                }

                /* If first visit */
                if ( stateAndActionShouldBeRecorded(
                        state
                     )
                ){
                    visitedStatesAndActions.emplace_back(state, agentDecision);
                }
            } 
            // else {
            //      // The only time it would be a nullptr is if the agent chose to stand previously
            //     agentDecision = STAND;
            // }

            testEnvironment.simulateNextRound(agentDecision);
            state = testEnvironment.getCurrentState();
        }

        // Generate the reward value from the result of the game
        float reward = generateRewardValue( state.getOutcome() );

        /* Bet 5 as long as there player has 5 to bet */
        if (currentWinnings >= 5){
            currentWinnings += reward * 5;
            
            highestWinnings = std::max(highestWinnings, currentWinnings);
        }

        cumulativeReward += reward;

        updateQValues(Q, visitedStatesAndActions, reward, 0.001f);

        cout << "Now sleeping for 5 seconds \n";
        // std::this_thread::sleep_for(milliseconds(5000));
        if (numberOfSimulations > 100 && (i % (numberOfSimulations / 100) == 0)) {
            auto timeLog = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(timeLog - start);

            std::clog << i << " simulations completed in " << duration.count() << "milliseconds\n\n";
        }
    }
}

void updateQValues(
    function::StateActionFunction &Q,
    std::vector<StateAndAction> &visitedStatesAndActions,
    float G,
    float learningFactor
){
    for (StateAndAction &p: visitedStatesAndActions){
        environment::GameState state = p.first;
        environment::Action action = p.second;
        cout << "Now updating the Q-Values for:\n" << state << "\n with action " << action << "\n using reward " << G << "\n";

        cout << "Q-Value before = " << *Q(state, action) << "\n";
        /* Calculate the updates to the Q-Value using the learning factor to prevent rapid and drastic changes */
        *Q(state, action) = *Q(state, action) + learningFactor * (G - *Q(state, action));

        cout << "Q-Value after = " << *Q(state, action) << "\n";

    }
    // Clear the vector for future calculations
    visitedStatesAndActions.clear();
}

void runEpisode(
    agents::PassiveAgent &agent, 
    environment::GameState &state, 
    environment::EnvironmentHandler &testEnvironment,    
    std::vector<StateAndAction> &visitedStatesAndActions, 
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
        if (stateAndActionShouldBeRecorded(
                state
            )
        ){

            cout << "State not visited before" << "\n";
            // cout << "(N) Count before incrementing = " << *N(state, agentDecision) << "\n";

            ++(*N(state, agentDecision));

            // cout << "(N) Count after incrementing = " << *N(state, agentDecision) << "\n";

            visitedStatesAndActions.emplace_back(state, agentDecision);
        } else {
            cout << "Redundant state or state visited before in this episode :\n";
            cout << state << "\n";
        }

        testEnvironment.simulateNextRound(agentDecision);
        state = testEnvironment.getCurrentState();
    }
}

void updateReturnSums(
    environment::GameState &state, 
    std::vector<StateAndAction> &visitedStatesAndActions, 
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


        visitedStatesAndActions.pop_back();
    }
}

void outputValueFunction(
    function::StateActionFunction &Q
){
    /*  Outputs the value function by combining the q-values for each action in a given state
        into a sum for the entire state, since the q-values are averages */
    cout << "Outputting in the form:\t``playertotal dealertotal maxValue;``\n\n";
    for (int k = 0; k < 2; ++k){
        cout << (k ? "With":"Without") << " usable ace.\n";
        for (int i = 12; i <= environment::MAX_PLAYER_TOTAL; ++i){
            for (int j = 2; j <= environment::MAX_DEALER_SHOWING; ++j){
                // Get the max value for a given state
                float value = std::max(*Q.getImage(i, j, 0, k), *Q.getImage(i, j, 1, k));
                
                cout << i << " " << j << " " << value;
                cout << (j == environment::MAX_DEALER_SHOWING ? "\n":";");
            }
        }
        cout << "\n";
    }
}
