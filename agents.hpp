#pragma once

#ifndef AGENTS_H

#define AGENTS_H

#include <utility>
#include "environment.hpp"

namespace agents{
    /*  The initial agent with a fixed policy.
        Stand on or above 18 with probability 80% and hit when below 18 on probability 80%
        Hit when below 12 with probability 100% because it is impossible to bust.
    */
    class PassiveAgent {
    private:
        /*  Q, N and returnSums store functions that keep track of the current state 
            The state is represented by the cards currently in play by both us and the dealer
            Holds current gameState and the action taken */
        float discountFactor;
        environment::Action action;
    public:
        PassiveAgent();

        virtual environment::Action policy(environment::GameState state);

        virtual environment::Action considerState(environment::GameState state);

        /* The agent resets its choice */
        virtual void reset();

    };
}


#endif /* AGENTS_H */