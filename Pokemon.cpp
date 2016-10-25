#include "Pokemon.h"
#include <iostream>

using namespace std;

Pokemon::Pokemon(int index, string name, vector<Evolution> evolutions, vector<Type> types, vector<Move> moves) :
	index_(index),
	name_(name),
	evolutions_(evolutions),
	types_(types),
	moves_(moves)
{}

Pokemon::~Pokemon()
{
}

int Pokemon::getPokemonId() const {
	return index_;
}

string Pokemon::getPokemonName() const {
	return name_;
}

vector<Evolution> Pokemon::getEvolutions() const {
	return evolutions_;
}

vector<string> Pokemon::typesToString() const{
	vector<string> result;
	
	for (Type t : types_) {

	}

	return result;
}

vector<Move> Pokemon::getMoves() const {
	return moves_;
}

vector<Pokemon::Type> Pokemon::stringToTypes(vector<string>& typesInString) {
	vector<Type> types;

	for (string type : typesInString) {
		Type t;

		// Let's reduce if statement filtering with chars
		// http://www.cplusplus.com/reference/string/string/at/
		switch (type.at(0)) {
		case 'b':
			t = Type::BUG;
			break;
		case 'd':
			if (type == "dark") {
				t = Type::DARK;
			}
			else if (type == "dragon") {
				t = Type::DRAGON;
			}
			break;
		case 'e':
			if (type == "electric") {
				t = Type::ELECTRIC;
			}
			break;
		case 'f':
			if (type == "fairy") {
				t = Type::FAIRY;
			}
			else if (type == "fighting") {
				t = Type::FIGHTING;
			}
			else if (type == "fire") {
				t = Type::FIRE;
			}
			else if (type == "flying") {
				t = Type::FLYING;
			}
			break;
		case 'g':
			if (type == "ghost") {
				t = Type::GHOST;
			} else if (type == "grass") {
				t = Type::GRASS;
			}
			else if (type == "ground") {
				t = Type::GROUND;
			}
			break;
		case 'i':
			if (type == "ice") {
				t = Type::ICE;
			}
			break;
		case 'n':
			if (type == "normal") {
				t = Type::NORMAL;
			}
			break;
		case 'p':
			if (type == "poison") {
				t = Type::POISON;
			}
			else if (type == "psychic") {
				t = Type::PSYCHIC;
			}
			break;
		case 'r':
			if (type == "rock") {
				t = Type::ROCK;
			}
			break;
		case 's':
			if (type == "steel") {
				t = Type::STEEL;
			}
			break;
		case 'w':
			if (type == "water") {
				t = Type::WATER;
			}
			break;
		default:
			cout << "Error detecting type for: " + type << endl;
			break;
		}

		types.push_back(t); // Push back the enum
	}

	return types;
}