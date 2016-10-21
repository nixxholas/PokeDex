#pragma once
#include "Pokemon.h"

class PokeDex
{
private:
	vector<Pokemon> Pokemons_;

public:
	// Returns all of the pokemons for use
	vector<Pokemon> getPokemons(); 
	
	// Setup the Pokemon Vector.
	void initializePokemons();

	// Switch case container for the options from cin
	void menuChoice(int);

	PokeDex();

	~PokeDex();
};

