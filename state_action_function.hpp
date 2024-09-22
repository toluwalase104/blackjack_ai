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

namespace state_action_function {

    using StateMatrix = std::array<std::array<std::array<float, 2>, 22>, 22>;

    class Function{
        public:
            Function();

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
            StateMatrix mapping;

    };

}

std::ostream& operator<<(std::ostream& o, state_action_function::Function &f);

#endif /* STATE_ACTION_FUNCTION_H */