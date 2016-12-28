#pragma once
#include "Pokemon.h"

class PokemonGo : public Pokemon {
private:
	string nickname_;
	int cp_;
	int hp_;
	int stardust_;
	int level_;

public:
	// Calculates the Potential of Individual Values of the PokemonGo Pokemon
	bool calculatePotentialIV() const;

	// Default Constructor
	PokemonGo(string, int, int, int, int,
		int, string, vector<Evolution>, vector<Type>, vector<Move>);

	// Destructor
	~PokemonGo();
};