#pragma once
#include <string>

using namespace std;

class Evolution {
private:
	int index; // The unique id of the pokemon we're evolving to. 
	string event; // The string to define what the pokemon has to meet in order to evolve.

public:
	// A constructor for Evolution
	Evolution(int index = 0, string event = "");
};