#include <string>
#include <iostream>
#include "Pokemon.h"
#include "Evolution.cpp"
#include "Move.cpp"

using namespace std;

Pokemon::Pokemon(int index, string name, string image_url,
				list<string> types, list<Move> moves) {
	Index = index;
	Name = name;
	Image_url = image_url;
	Types = types;
	list<Move> Moves = moves;
}

// Destructor
Pokemon::~Pokemon() {

}

void Pokemon::printAllMoveNames() const {
	/*for (list<Move>::iterator iterator = this->Moves.begin(); iterator != this->Moves.end(); ++iterator) {
		cout << iterator->getMoveName();
	}*/
	/*for (auto const& i : this->Moves) {
		cout << i.getMoveName() << endl;
	}*/
}