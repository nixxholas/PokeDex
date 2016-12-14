#pragma once
#include <string>
#include <regex>
#include <map>
#include "rapidjson/document.h"
#include "Pokemon.h"
#include "PokemonGo.h"
#include "LevelData.h"

class PokeDex
{
private:
	bool exitStatus;
	vector<Pokemon> Pokemons_;
	rapidjson::Document* document_ = new rapidjson::Document();
	// https://www.tutorialspoint.com/cplusplus/cpp_multi_dimensional_arrays.htm
	//Pokemon::Type typeEffectiveness[][]; // Screw this, not gonna waste time on that
	vector<LevelData> levelsData_; // Stores all the Data proper for PoGo Algo
	vector<PokemonGo> PoGomons_; // Stores all the PokemonGo Pokemons

public:
	// Setup the PokemonGo LevelData Vector
	void initializeLevelData();

	// initializeLevelData()'s Worker Task
	void iLDThreadTask(const rapidjson::Value&);

	// Setup the Pokemon Vector.
	void initializePokemons();

	// Returns the size of the Pokemon vector in bytes.
	int getBytesOfPokemons() const;

	// initializePokemons()'s Worker Task
	void iPThreadTask(const rapidjson::Value&);

	// Save the Pokemon Vector.
	void savePokemons();

	// savePokemons()'s Worker Task
	void sPThreadTask(const Pokemon&);

	// Function to retrieve pokemon via it's ID
	Pokemon* getPokemonById(int);

	// Function to display the search menu
	void launchSearchMenu();

	// Function to search pokemon via substring name
	vector<Pokemon*> searchWithName();

	// Function to search pokemon via Type
	vector<Pokemon*> searchWithType(Pokemon::Type);

	// Function to select pokemon from the results
	Pokemon* selectPokemonFromResults(vector<Pokemon*>&);

	// Function to create a new pokemon
	void launchCreatePokemon();

	// Function to edit an existing pokemon
	void launchEditPokemon(Pokemon&);

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

	PokeDex::PokeDex();

	PokeDex::~PokeDex();
};

