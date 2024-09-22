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
    public:
        PassiveAgent();

        virtual environment::Action policy(environment::GameState state);

        virtual environment::Action considerState(environment::GameState state);

        /* The agent resets its choice */
        virtual void reset();

    private:
        /* The action the agent chooses at a given moment */
        environment::Action action;
    };
}


#endif /* AGENTS_H */