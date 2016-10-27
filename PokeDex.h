#pragma once
#include <string>
#include <regex>
#include "rapidjson/document.h"
#include "Pokemon.h"

class PokeDex
{
private:
	bool exitStatus;
	vector<Pokemon> Pokemons_;
	vector<Pokemon> unstagedPokemons_; // Do I need this?
	Document* exportDocument = new Document();

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

