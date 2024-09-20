float getRandomFloat() {
    return (float)(rand()) / (float)(RAND_MAX);
}

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