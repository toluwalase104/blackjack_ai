#include "function.hpp"

/* Calls helper funciton to initialises all images */
function::StateActionFunction::StateActionFunction(){
    this->initialiseImages();
}

/* Returns the memory location that a given state and action is mapped to internally */
float* function::StateActionFunction::operator()(environment::GameState state, environment::Action action){
    int row = state.getPlayerTotal();

    int column = state.getFaceupTotal();

    // 1 for a hit action and 0 for a stand action
    int slice = action == environment::Action::HIT;

    // 1 for yes, 0 for no
    int frame = state.doesPlayerHaveUsableAce();

    return this->getImage(row, column, slice, frame);
}

/* Uses "4d" iterators to return the float at the specified memory location */
float* function::StateActionFunction::getImage(int i, int j, int k, int l){

    // If any of the indices given are out of bounds, do not attempt to access pointers
    // At most 2 actions, so action value must be 0 or at most 1
    if ( i < 0 || j < 0 || k < 0 ||
         i > environment::MAX_PLAYER_TOTAL ||
         j > environment::MAX_DEALER_SHOWING ||
         k >= environment::MAX_POSSIBLE_ACTIONS ||
         (l != 1 && l != 0) )
    {
            std::cout << "Image cannot be returned because indices are out of bound\n";
            std::cout << "i = " << i << ", j = " << j << ", k = " << k << ", l = " << l << "\n";
            return nullptr;
    }

    return &this->mapping[i][j][k][l];
}
// The initial significantly harder to read code for the above function
// return *( (*( (*(this->mapping.begin() + i)).begin() + j)).begin() + k);

/* Sets all function outputs equal to 0 */
void function::StateActionFunction::initialiseImages(){
    for (int i = 0; i <= environment::MAX_PLAYER_TOTAL; ++i){
        for (int j = 0; j <= environment::MAX_DEALER_SHOWING; ++j){
            for (int k = 0; k < environment::MAX_POSSIBLE_ACTIONS; ++k){
                // Initialise all images values to be 0
                this->mapping[i][j][k][0] = this->mapping[i][j][k][1] = 0.0f;
            }
        }
    }
}

/* Outputs the state */
std::ostream& operator<<(std::ostream& o, function::StateActionFunction &func){
    std::cout << "The state (S) consists of the player sum (p) and the shown dealer sum (d).\n"<<
                 "The reward (G) is given for when the agent hits (h) and stands (s).\n\n";

    for (int k = 0; k < 2; ++k){
        std::cout << "When the player " << (k ? "had":"did not have") << " a usable ace:\n";
        for (int i = 12; i <= environment::MAX_PLAYER_TOTAL; ++i){
            for (int j = 2; j <= environment::MAX_DEALER_SHOWING; ++j){
                std::cout << "(S = {p: " << i <<
                                ", d: " << j <<
                                "}; G = {h: " << *func.getImage(i, j, 1, k) << 
                                ", s: " << *func.getImage(i, j, 0, k) << "} ) ";
            }
            std::cout << "\n";
        }
    }
    
    return o;
}