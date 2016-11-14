#include "PokemonGo.h"

// Default Constructor
//
// The default constructor is being overloaded together with the parent
// class
PokemonGo::PokemonGo(string nickname, int cp, int hp, int stardust, int level, Grade grade,
	int index, string name, vector<Evolution> evolutions, vector<Type> types, vector<Move> moves) :
	Pokemon(index, name, evolutions, types, moves),
	nickname_(nickname),
	cp_(cp),
	hp_(hp),
	stardust_(stardust),
	level_(level),
	grade_(grade) {

}

// Destruction
PokemonGo::~PokemonGo() {};