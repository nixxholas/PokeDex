#include "PokemonGo.h"
#include <iostream>
#include <limits>

// Default Constructor
//
// The default constructor is being overloaded together with the parent
// class
PokemonGo::PokemonGo(string nickname, int cp, int hp, int stardust, int level,
	int index, string name, vector<Evolution> evolutions, vector<Type> types, vector<Move> moves) :
	Pokemon(index, name, evolutions, types, moves),
	nickname_(nickname),
	cp_(cp),
	hp_(hp),
	stardust_(stardust),
	level_(level) { }

// Destruction
PokemonGo::~PokemonGo() {};

 bool PokemonGo::calculatePotential(LevelData& levelData) const {
	std::cout << "Candy required for the level: " << levelData.getCandy() << std::endl;
	std::cout << "Stardust required for the level: " << levelData.getDust() << std::endl;

	// http://stackoverflow.com/questions/903221/press-enter-to-continue
	std::cout << "Press Enter to Continue";
	if (std::cin.get())
	return true;
}

int PokemonGo::getLevel() const {
	return level_;
}