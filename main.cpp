#include <thread>
#include <chrono>

#include "agents.hpp"

using std::cout;
using std::cin;
using std::vector;
using namespace std::chrono;
// using namespace environment;

int main() {
    srand(time(0));
    cout << "Enter the number of simulations: ";
    int numberOfSimulations;
    cin >> numberOfSimulations;
    cout << "\n";

    // Clips the number of simulations to be between one and twenty
    numberOfSimulations = std::max(1, std::min(numberOfSimulations, 20));

    // Initialise the passive agent
    agents::PassiveAgent agent;

    bool agentDecision;

    int statesSeen = 0;

    for (int i = 1; i <= numberOfSimulations; ++i) {
        cout << "SIMULATION #" << i << ":\n";
        environment::EnvironmentHandler testEnvironment;
        environment::GameState state = testEnvironment.getCurrentState();

        agent.reset();
        // cout << testEnvironment.getCurrentState().getNumberOfSeenCards() << " cards seen.\n";

        while (state.getOutcome() == environment::GameResult::Unfinished) {

            agentDecision = agent.considerNextState(state);
            ++statesSeen;
            cout << "\n" << (agentDecision ? "The agent chooses to hit." : "The agent chooses to stand.") << "\n";

            testEnvironment.simulateNextRound(agentDecision);
            state = testEnvironment.getCurrentState();
        }
        // Output the final game outcome
        cout << "Ultimate outcome: " << state.getOutcome() << "\n";

        cout << "Now sleeping for 10 seconds \n";
        std::this_thread::sleep_for(milliseconds(10000));        
    }

    // NOTE: Need to implement hash function to store states-action pairs with their results and the returns_sums
    cout << "\nNow printing agent's stored states\n=====================================\n\n";
    vector<std::pair<environment::GameState, bool>> visitCounts = agent.getVisitCounts();

    for (auto& p : visitCounts) {
        cout << p.first << (p.second ? "HIT" : "STAND") << "\n=====================================\n\n";
    }
    cout << "Number of counted states = " << statesSeen << " VS Number of states stored = " << visitCounts.size() << "\n";

    return 0;
}