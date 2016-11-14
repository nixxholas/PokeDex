#pragma once
#include "Pokemon.h"
#include "Grade.h"

class PokemonGo : public Pokemon {
private:
	string nickname_;
	int cp_;
	int hp_;
	int stardust_;
	int level_;
	Grade grade_;

public:
	// Calculates the Potential of Individual Values of the PokemonGo Pokemon
	void calculatePotentialIV() const;

	// Default Constructor
	PokemonGo(string, int, int, int, int, Grade,
		int, string, vector<Evolution>, vector<Type>, vector<Move>);

	// Destructor
	~PokemonGo();
};