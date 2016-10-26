#pragma once

#include <string>
#include <vector>

#include "Evolution.h"
#include "Move.h"

using namespace std;

class Pokemon
{
public:
	// Has to be public for PokeDex's Accessibility
	enum Type {
		BUG, // 0 
		GRASS, // 1
		DARK, // 2
		GROUND, // 3
		DRAGON, // 4
		ICE, // 5
		ELECTRIC, // 6
		NORMAL, // 7
		FAIRY, // 8
		POISON, // 9
		FIGHTING, // 10
		PSYCHIC, // 11
		FIRE, // 12
		ROCK, // 13
		FLYING, // 14
		STEEL, // 15
		GHOST, // 16
		WATER // 17
	};

private:
	int index_;
	string name_;
	vector<Evolution> evolutions_;
	//vector<string> types_;
	vector<Move> moves_;
	vector<Type> types_;

public:
	// Returns a vector of enum Types with the vector of strings
	static vector<Type> stringToTypes(vector<string>&);

	// Returns int pokemon
	int getPokemonId() const;

	// Returns the vector of evolutions
	vector<Evolution> getEvolutions() const;

	// Returns the pokemon name
	string getPokemonName() const;

	// Returns the types in a vector of string
	vector<string> typesToString() const;
	
	// Returns the vector of moves
	vector<Move> getMoves() const;

	// Returns us the pokemon type chosen
	static Type chooseTypeEnum();

	// Returns us the enum in string
	static Type enumIntToType(int);

	// Sets the pokemon id to evolve to
	void setPokemonId(int Pokemon);

	// Sets the Pokemon evolution event string
	void setEvent(string Event);

	// Parameterized Constructor
	Pokemon(int, string, vector<Evolution>, vector<Type>, vector<Move>);

	// Destructor for Pokemon
	~Pokemon();

};

