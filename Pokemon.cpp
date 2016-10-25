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
		switch (t) {
		case Type::BUG: // 0
			result.push_back("bug");
			break;
		case Type::GRASS: // 1
			result.push_back("grass");
			break;
		case Type::DARK: // 2
			result.push_back("dark");
			break;
		case Type::GROUND: // 3
			result.push_back("ground");
			break;
		case Type::DRAGON: // 4
			result.push_back("dragon");
			break;
		case Type::ICE: // 5
			result.push_back("ice");
			break;
		case Type::ELECTRIC: // 6
			result.push_back("electric");
			break;
		case Type::NORMAL: // 7
			result.push_back("normal");
			break;
		case Type::FAIRY: // 8
			result.push_back("fairy");
			break;
		case Type::POISON: // 9
			result.push_back("poison");
			break;
		case Type::FIGHTING: // 10
			result.push_back("fighting");
			break;
		case Type::PSYCHIC: // 11
			result.push_back("psychic");
			break;
		case Type::FIRE: // 12
			result.push_back("fire");
			break;
		case Type::ROCK: // 13
			result.push_back("rock");
			break;
		case Type::FLYING: // 14
			result.push_back("flying");
			break;
		case Type::STEEL: // 15
			result.push_back("steel");
			break;
		case Type::GHOST: // 16
			result.push_back("ghost");
			break;
		case Type::WATER: // 17
			result.push_back("water");
			break;
		default:
			cout << "Failed to parse a Type enum." << endl;
			break;
		}
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