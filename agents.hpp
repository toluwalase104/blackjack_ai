#pragma once

#ifndef AGENTS_H

#define AGENTS_H

#include <utility>
#include "environment.hpp"

namespace agents{
    /*  The initial agent with a fixed policy.
        Stand on or above 18 with probability 80% and hit when below 18 on probability 80%*/
    class PassiveAgent {
    private:
        /*  Q, N and returnSums store functions that keep track of the current state 
            The state is represented by the cards currently in play by both us and the dealer
            Holds current gameState and the action taken */
        std::vector<std::pair<environment::GameState, bool>> Q, N, returnSums;
        float discountFactor;
        bool hit;
    public:
        PassiveAgent();

        virtual bool policy(environment::GameState state);

        virtual void addDecision(environment::GameState state, bool action);

        virtual bool considerNextState(environment::GameState state);

        std::vector<std::pair<environment::GameState, bool>> getVisitCounts();

        std::vector<std::pair<environment::GameState, bool>> getReturnSums();

        /* The agent resets its choice */
        virtual void reset();

    };
}


#endif /* AGENTS_H */