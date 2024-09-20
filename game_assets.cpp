/*  Currently Aces are treated as just 1 so additional features
    need to be implemented to handle usable Aces. */
enum class CardVal :int {
    Ace = 1,
    Two = 2, Three = 3, Four = 4,
    Five = 5, Six = 6, Seven = 7,
    Eight = 8, Nine = 9, Ten = 10,
    Jack = 10, Queen = 10, King = 10
}card_value;

std::ostream& operator<<(std::ostream& o, CardVal v) {
    cout << static_cast<int>(v);
    return o;
}

enum class Suite :char {
    Hearts = 'H',
    Diamonds = 'D',
    Spades = 'S',
    Clubs = 'C'
}card_suite;

std::ostream& operator<<(std::ostream& o, Suite s) {
    cout << static_cast<char>(s);
    return o;
}



class Card {
private:
    int id;
    CardVal value;
    Suite suite;
public:
    Card() {

    }

    Card(int id, CardVal value, Suite suite) :
        id(id), value(value), suite(suite) {
    }

    int getID() {
        return id;
    }

    CardVal getValue() {
        return value;
    }

    Suite getSuite() {
        return suite;
    }

};

std::ostream& operator<<(std::ostream& o, Card c) {
    cout << "{ Value = " << c.getValue() << ", Suite = " << c.getSuite() << "} ";
    return o;
}