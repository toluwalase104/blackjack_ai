#include "function.hpp"

/* Calls helper funciton to initialises all images */
function::StateActionFunction::StateActionFunction(){
    this->initialiseImages();
}

/* Returns the memory location until */
float* function::StateActionFunction::operator()(environment::GameState state, environment::Action action){
    int row = state.getPlayerTotal();

    int column = state.getFaceupTotal();

    // 1 for a hit action and 0 for a stand action
    int slice = (action == environment::Action::HIT ? 1:0);

    return this->getImage(row, column, slice);
}

/* Uses 3d iterators to return the integer at the specified memory location */
float* function::StateActionFunction::getImage(int i, int j, int k){

    // If any of the indices given are out of bounds, do not attempt to access pointers
    // At most 2 actions, so action value must be 0 or at most 1
    if ( i < 0 || j < 0 || k < 0 || 
         i > environment::MAX_PLAYER_TOTAL ||
         j > environment::MAX_DEALER_SHOWING ||
         k >= environment::MAX_POSSIBLE_ACTIONS  )
    {
            std::cout << "Image cannot be returned because indices are out of bound\n";
            std::cout << "i = " << i << ", j = " << j << ", k = " << k << "\n";
            return nullptr;
    }

    return &this->mapping[i][j][k];
}
// The initial significantly harder to read code for the above function
// return *( (*( (*(this->mapping.begin() + i)).begin() + j)).begin() + k);

/* Sets all function outputs equal to 0 */
void function::StateActionFunction::initialiseImages(){
    for (int i = 0; i <= environment::MAX_PLAYER_TOTAL; ++i){
        for (int j = 0; j <= environment::MAX_DEALER_SHOWING; ++j){
            for (int k = 0; k < environment::MAX_POSSIBLE_ACTIONS; ++k){
                this->mapping[i][j][k] = 0.0f;
            }
        }
    }
}

/* Outputs the state */
std::ostream& operator<<(std::ostream& o, function::StateActionFunction &func){
    std::cout << "The state (S) consists of the player sum (p) and the shown dealer sum (d).\n"<<
                 "The reward (G) is given for when the agent hits (h) and stands (s).\n\n";

    for (int i = 12; i <= environment::MAX_PLAYER_TOTAL; ++i){
        for (int j = 1; j <= environment::MAX_DEALER_SHOWING; ++j){
            std::cout << "(S = {p: " << i <<
                            ", d: " << j <<
                            "}; G = {h: " << *func.getImage(i, j, 1) << 
                            ", s: " << *func.getImage(i, j, 0) << "} ) ";
        }
        std::cout << "\n";
    }
    
    return o;
}