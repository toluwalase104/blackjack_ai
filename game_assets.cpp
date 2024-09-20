#include "game_assets.hpp"

using std::cout;
// using namespace game_assets;

game_assets::Card::Card(int id, CardVal value, Suite suite) :
        id(id), value(value), suite(suite) {}

int game_assets::Card::getID() const{
    return game_assets::Card::id;
}

game_assets::CardVal game_assets::Card::getValue() const{
    return game_assets::Card::value;
}

game_assets::Suite game_assets::Card::getSuite() const{
    return game_assets::Card::suite;
}

std::ostream& operator<<(std::ostream& o, game_assets::CardVal v) {
    cout << static_cast<int>(v);
    return o;
}

std::ostream& operator<<(std::ostream& o, game_assets::Suite s) {
    cout << static_cast<char>(s);
    return o;
}

std::ostream& operator<<(std::ostream& o, game_assets::Card c) {
    cout << "{ Value = " << c.getValue() << ", Suite = " << c.getSuite() << "} ";
    return o;
}