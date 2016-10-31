#pragma once
#include <string>
#include <regex>
#include <map>
#include "rapidjson/document.h"
#include "Pokemon.h"

class PokeDex
{
private:
	bool exitStatus;
	vector<Pokemon> Pokemons_;
	rapidjson::Document* document_;

public:
	// Setup the Pokemon Vector.
	void initializePokemons();

	// initializePokemons()'s Worker Task
	void iPThreadTask(const rapidjson::Value&);

	// Save the Pokemon Vector.
	void savePokemons();

	// savePokemons()'s Worker Task
	void sPThreadTask(const Pokemon&);

	// Function to display the search menu
	void launchSearchMenu();

	// Function to search and return the pokemon select in integer
	int searchAndGetPokemonIndex();

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

	// Evolution Object Creator Console Function
	Evolution createEvolution();

	// Move Object Creator Console Function
	Move createMove();

	// Adds a pokemon to the unstagedPokemons Vector
	void addUnstagedPokemon(Pokemon);

	// Removes a pokemon from the unstagedPokemons Vector
	void removeUnstagedPokemon(Pokemon);

	PokeDex::PokeDex();

	PokeDex::~PokeDex();
};

