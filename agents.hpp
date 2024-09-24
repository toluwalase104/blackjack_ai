#pragma once

#ifndef AGENTS_H

#define AGENTS_H

#include <utility>
#include "environment.hpp"
#include "function.hpp"

namespace agents{
    /*  The initial agent with a fixed policy.
        Stand on or above 18 with probability 80% and hit when below 18 on probability 80%
        Hit when below 12 with probability 100% because it is impossible to bust.
    */

   /* ABSTRACT Agent class */
   class Agent {
    public:
        Agent();

        virtual environment::Action considerState(environment::GameState state) = 0;

        /* The agent resets its choice */
        virtual inline void reset(){
            this->action = environment::Action::HIT;
        }
    protected:
        /* The action the agent chooses at a given moment */
        environment::Action action;  
    private:
        /* pure virtual function to be implemented as the agent initially has no policy */
        virtual environment::Action policy(environment::GameState state) = 0;

    };
    //

    class PassiveAgent: public Agent {
    public:
        PassiveAgent();

        environment::Action considerState(environment::GameState state);
    private:

        virtual environment::Action policy(environment::GameState state);
    };

    /* The minimum probability of choosing a random action as opposed to the currently optimal. */
    const float EPSILON_MIN = 0.01;

    /* This agent makes its decisions using epsilon-greedy to determine whether or not it should
        greedily take the current best action for the state or attempt to take a different one*/
    class GreedyAgent: public Agent {
        public:
            GreedyAgent();

            /* Takes epsilon, the rate of decay of epsilon and a pointer to the optimal function*/
            GreedyAgent(float epsilon, float decayRate);

            environment::Action considerState(environment::GameState state);

            void setActionValues(float hitValue, float standValue);

            inline environment::Action getAction(){
                return this->action;
            }

        private:
            /* Epsilon holds the probability of choosing a random action, in a given state*/
            float epsilon, decayRate, hitValue, standValue;

            environment::Action policy(environment::GameState state);
    };
    
}


#endif /* AGENTS_H */