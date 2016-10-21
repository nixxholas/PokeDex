#include "Evolution.h"

using namespace std;

Evolution::Evolution() {}

int Evolution::getPokemonId() const {
	return this->Pokemon;
}

string Evolution::getEvolvingEvent() const {
	return this->Event;
}

void Evolution::setPokemonId(int pokemon) {
	this->Pokemon = pokemon;
}

void Evolution::setEvent(string event) {
	this->Event = event;
}