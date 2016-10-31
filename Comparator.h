#pragma once
#include "Pokemon.h"

class Comparator : public Pokemon {
private:
	Pokemon pokemonToCompare_;

public:
	// Prints out the comparison between two pokemons
	void comparePokemons();
	
	// Custom Default Constructor
	Comparator::Comparator(Pokemon);

	Comparator::~Comparator();
};