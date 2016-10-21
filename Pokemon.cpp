#include "Pokemon.h"



Pokemon::Pokemon(int index, string name, vector<Evolution> evolutions, vector<string> types, vector<Move> moves) :
	index_(index),
	name_(name),
	evolutions_(evolutions),
	types_(types),
	moves_(moves)
{}

Pokemon::~Pokemon()
{
}


string Pokemon::getPokemonName() const{
	return name_;
}