#include "Pokemon.h"

using namespace std;

Pokemon::Pokemon(int index, int name, vector<string> types, vector<Move> moves) {
	Index = index;
	Name = name;
	Types = types;
	Moves = moves;
}

Pokemon::~Pokemon() {}