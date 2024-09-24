#pragma once

#ifndef STATE_ACTION_FUNCTION_H

#define STATE_ACTION_FUNCTION_H

#include <unordered_map>
#include <utility>
#include "environment.hpp"

/* PLAYERCARDS AND DEALER CARDS IN GAMESTATE CAN BE IMPLEMENTED AS A VECTOR OF INTEGERS 
    WITH A ONE REPRESENTING THE PRESCENCE OF AN ITEM AND A 0 REPRESENTING THE ABSENCE OF A CARD.
    THIS WOULD ALLOW A HASH FUNCTION TO BE COMPUTED IN O(1) TIME SINCE BOTH THE DEALER 
    AND THE PLAYER HAVE <= 52 CARDS COMBINED  */ 
/* Maps state and an action pair to a value (the cumulative reward) */ 

namespace function {

    /*  3D array that maps a state (represented by the player sum and dealer sum)
        and an action (represented by hit or stand) to a float.
        It would be smarter to convert this into a template for mapping to arbitrary types.*/
    template <typename T>
    using StateActionMatrix = 
        std::array<
            std::array<
                std::array<T, environment::MAX_POSSIBLE_ACTIONS>,
                environment::MAX_DEALER_SHOWING + 1
            >,
            environment::MAX_PLAYER_TOTAL + 1
        >;

    class StateActionFunction{
        public:
            StateActionFunction();

            /* Allows the state to be stored numerically */
            float* operator()(environment::GameState state, environment::Action action);

            /* Returns the image of a given function input */
            float* getImage(int i, int j, int k);

            void initialiseImages();
        private:
            /*  A 3D array that stores actions taken in each state for the function
                Stores rows for the player sum from i = 0 to i = 21,
                Stores columns for the dealer sum from i = 0 to i = 21,
                Stores the action taken during the state where 0 = stand and 1 = hit. */
            StateActionMatrix<float> mapping;

    };

    /* Implement a state function super class that allows a state to be mapped to an arbitrary type */
}

std::ostream& operator<<(std::ostream& o, function::StateActionFunction &f);

#endif /* STATE_ACTION_FUNCTION_H */