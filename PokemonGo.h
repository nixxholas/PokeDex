#pragma once
#include "Pokemon.h"
#include "LevelData.h"

class PokemonGo : public Pokemon {
private:
	string nickname_;
	int cp_;
	int hp_;
	int stardust_;
	int level_;

public:
	// Calculates the Potential of Individual Values of the PokemonGo Pokemon
	bool calculatePotentialIV(LevelData&) const;

	// Retrieves the level of the PoGomon
	int getLevel() const;

	// Default Constructor
	PokemonGo(string, int, int, int, int,
		int, string, vector<Evolution>, vector<Type>, vector<Move>);

	// Destructor
	~PokemonGo();
};