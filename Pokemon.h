#pragma once
#include <iostream>
#include <list>
#include "Move.h"
#include "Evolution.h"

using namespace std;

class Pokemon {
private:
	int Index; // Gonna store the unique id here
	string Name; // Name of the pokemon, Bulbasaur
	string Image_url; // Url to pull for the pokemon's image for fun
	// http://codereview.stackexchange.com/questions/41525/creating-objects-and-putting-them-into-stdlist
	list<string> Types; // Types, Grass type etc.
	int Evolution; // What pokemon the current pokemon can potentially evolve to.
	list<Move> Moves; // The moves this pokemon will have

public:
	// Returns the list of moves this pokemon will be able to acquire
	void printAllMoveNames() const;

	// Set the Pokemon's Evolution.
	void setEvolution(int index);

	// The Constructor design for a Pokemon.
	Pokemon(int index, string name, string image_url,
			list<string> types, list<Move> moves);

	// Destructor for Pokemon
	~Pokemon();
};