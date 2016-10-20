#pragma once
#include <string>

using namespace std;

class Move {
private:
	int level; // Level requirement for the move. If it's default, then it'll be 0.
	string name; // Name of the move
	string type; // Type of the move
	string category; // Category of the move
	int attack; // Damage of the move
	int accuracy; // Accuracy of the move max 100, min 1;
	int pp; // Amount of times this move can be used unless replenished
	int effect_percent; // Percentage change to instantiate DOT effect
	string description; // Description of the move.

public:
	string getMoveName();

	// Default Constructor we would want for the Move Object.
	Move(int level, string name, string type, string category, int attack, int accuracy, int pp, int effect_percent, string description);

};