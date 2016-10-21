#pragma once

#include <string>
#include <vector>

#include "Evolution.h"
#include "Move.h"

using namespace std;

class Pokemon
{
private:
	int index_;
	string name_;
	vector<Evolution> evolutions_;
	vector<string> types_;
	vector<Move> moves_;

public:
	// Returns int pokemon
	int getPokemonId() const;

	// Returns string Event
	string getEvolvingEvent() const;

	string getPokemonName() const;

	// Sets the pokemon id to evolve to
	void setPokemonId(int Pokemon);

	// Sets the Pokemon evolution event string
	void setEvent(string Event);

	// Parameterized Constructor
	Pokemon(int, string, vector<Evolution>, vector<string>, vector<Move>);

	// Destructor for Pokemon
	~Pokemon();
};

