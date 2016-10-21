#pragma once
#include <string>

using namespace std;

class Evolution {
private:
	int Pokemon; // The pokemon you're evolving to
	string Event; // The requirement of the evolution

public:
	// Returns int pokemon
	int getPokemonId() const;

	// Returns string Event
	string getEvolvingEvent() const;

	// Sets the pokemon id to evolve to
	void setPokemonId(int Pokemon);

	// Sets the Pokemon evolution event string
	void setEvent(string Event);

	// Default Constructor
	Evolution::Evolution();
};