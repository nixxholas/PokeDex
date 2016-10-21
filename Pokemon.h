#pragma once
#include <string>
#include <vector>
#include "Evolution.h"
#include "Move.h"

using namespace std;

class Pokemon {
private:
	int Index;
	string Name;
	vector<string> Types;
	Evolution Evolution();
	vector<Move> Moves;

public:

	// Default Constructor
	Pokemon::Pokemon(int index, string name, vector<string> types, vector<Move> moves);

	// Destructor for Pokemon
	Pokemon::~Pokemon();
};