#pragma once
#include "Pokemon.h"

class PokeDex
{
private:
	vector<Pokemon> Pokemons_;

public:
	// Setup the Pokemon Vector.
	void initializePokemons();

	// Switch case container for the options from cin
	void menuChoice(int);

	PokeDex();

	~PokeDex();
};

