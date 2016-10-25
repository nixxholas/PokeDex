#pragma once
#include <string>
#include <regex>
#include "Pokemon.h"

class PokeDex
{
private:
	bool exitStatus;
	vector<Pokemon> Pokemons_;
	vector<Pokemon> unstagedPokemons_;

public:
	// Setup the Pokemon Vector.
	void initializePokemons();

	// Save the Pokemon Vector.
	void savePokemons();

	// Function to display the search menu
	void launchSearchMenu();

	// Function to create a new pokemon
	void launchCreatePokemon();

	// Function to edit an existing pokemon
	void launchEditPokemon();

	// Function to remove an existing pokemon
	void launchDeletePokemon();

	// Function to display the main menu
	void launchMenu();

	// Switch case container for the options from cin
	void menuChoice(int&);

	// Adds a pokemon to the unstagedPokemons Vector
	void addUnstagedPokemon(Pokemon);

	// Removes a pokemon from the unstagedPokemons Vector
	void removeUnstagedPokemon(Pokemon);

	PokeDex();

	~PokeDex();
};

