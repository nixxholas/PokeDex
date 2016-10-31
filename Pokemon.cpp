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

int Pokemon::getTypesSize() const {
	return types_.size();
}

bool Pokemon::operator==(const string& name) const {
	return (name_ == name);
}

vector<Evolution> Pokemon::getEvolutions() const {
	return evolutions_;
}

vector<Pokemon::Type> Pokemon::getTypesVector() const {
	return types_;
}

vector<string> Pokemon::typesToString() const {
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

// Compared to typesToString, we're reducing the function into
// a Linear Incremental Difficulty O(N) Function instead of 
// an exponentially difficult (O2N) function
vector<int> Pokemon::typesToInt() const {
	vector<int> result;

	for (Type t : types_) {
		result.push_back(static_cast<int>(t));
	}

	return result;
}

int Pokemon::typeToInt(const Type& t) const {
	return static_cast<int>(t);
}

vector<Move> Pokemon::getMoves() const {
	return moves_;
}

// couts a list of Enumerators for the user to cin via enumIntToString()
// Checks the input, and eventually returns a string
Pokemon::Type Pokemon::chooseTypeEnum() {
	int choice;

	for (int i = Type::BUG; i != Type::WATER; ++i) {
		switch (i) {
		case Type::BUG: // 0
			cout << "(1) Bug" << endl;
			break;
		case Type::GRASS: // 1
			cout << "(2) Grass" << endl;
			break;
		case Type::DARK: // 2
			cout << "(3) Dark" << endl;
			break;
		case Type::GROUND: // 3
			cout << "(4) Ground" << endl;
			break;
		case Type::DRAGON: // 4
			cout << "(5) Dragon" << endl;
			break;
		case Type::ICE: // 5
			cout << "(6) Ice" << endl;
			break;
		case Type::ELECTRIC: // 6
			cout << "(7) Electric" << endl;
			break;
		case Type::NORMAL: // 7
			cout << "(8) Normal" << endl;
			break;
		case Type::FAIRY: // 8
			cout << "(9) Fairy" << endl;
			break;
		case Type::POISON: // 9
			cout << "(10) Poison" << endl;
			break;
		case Type::FIGHTING: // 10
			cout << "(11) Fighting" << endl;
			break;
		case Type::PSYCHIC: // 11
			cout << "(12) Psychic" << endl;
			break;
		case Type::FIRE: // 12
			cout << "(13) Fire" << endl;
			break;
		case Type::ROCK: // 13
			cout << "(14) Rock" << endl;
			break;
		case Type::FLYING: // 14
			cout << "(15) Flying" << endl;
			break;
		case Type::STEEL: // 15
			cout << "(16) Steel" << endl;
			break;
		case Type::GHOST: // 16
			cout << "(17) Ghost" << endl;
			break;
		case Type::WATER: // 17
			cout << "(18) Water" << endl;
			break;
		default:
			cout << "Failed to parse a Type enum." << endl;
			break;
		}
	}

	cin >> choice;
	while (!(choice < 18) || !(choice > 0)) {
		cout << "Please enter a valid input." << endl;
		cin >> choice;
	}

	return enumIntToType(choice - 1);
}

// Returns the string of the Enumerator identified by an Integer
Pokemon::Type Pokemon::enumIntToType(int inType) {
	return Type(inType); // http://www.gamedev.net/topic/330476-cc-how-to-convert-int-to-enum/
	//switch (inType) {
	//case Type::BUG: // 0
	//	return "bug";
	//	break;
	//case Type::GRASS: // 1
	//	return "grass";
	//	break;
	//case Type::DARK: // 2
	//	return "dark";
	//	break;
	//case Type::GROUND: // 3
	//	return "ground";
	//	break;
	//case Type::DRAGON: // 4
	//	return "dragon";
	//	break;
	//case Type::ICE: // 5
	//	return "ice";
	//	break;
	//case Type::ELECTRIC: // 6
	//	return "electric";
	//	break;
	//case Type::NORMAL: // 7
	//	return "normal";
	//	break;
	//case Type::FAIRY: // 8
	//	return "fairy";
	//	break;
	//case Type::POISON: // 9
	//	return "poison";
	//	break;
	//case Type::FIGHTING: // 10
	//	return "fighting";
	//	break;
	//case Type::PSYCHIC: // 11
	//	return "psychic";
	//	break;
	//case Type::FIRE: // 12
	//	return "fire";
	//	break;
	//case Type::ROCK: // 13
	//	return "rock";
	//	break;
	//case Type::FLYING: // 14
	//	return "flying";
	//	break;
	//case Type::STEEL: // 15
	//	return "steel";
	//	break;
	//case Type::GHOST: // 16
	//	return "ghost";
	//	break;
	//case Type::WATER: // 17
	//	return "water";
	//	break;
	//default:
	//	cout << "Failed to parse a Type enum." << endl;
	//	break;
	//}
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
			}
			else if (type == "grass") {
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

ostream & operator<<(ostream& ostreamObj, const Pokemon& pokemon)
{
	ostreamObj << pokemon.getPokemonName();

	return ostreamObj;
}

istream & operator >> (istream& istreamObj, const Pokemon& pokemon) {
	return istreamObj;
}
