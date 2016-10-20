#include <string>
#include <iostream>
#include "Pokemon.h"
#include "Evolution.h"
#include "Move.h"

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
	list<Move>::const_iterator iterator;
	for (iterator = this->Moves.begin(); iterator != this->Moves.end(); ++iterator) {
		
	}
}