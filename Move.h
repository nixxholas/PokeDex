#pragma once
#include <string>

using namespace std;

class Move {
private:
	int Level; // Level requirement for the move. If it's default, then it'll be 0.
	string Name; // Name of the move
	string Type; // Type of the move
	string Category; // Category of the move
	int Attack; // Damage of the move
	int Accuracy; // Accuracy of the move max 100, min 1;
	int Pp; // Amount of times this move can be used unless replenished
	int Effect_percent; // Percentage change to instantiate DOT effect
	string Description; // Description of the move.

public:
	string getMoveName() const;

	// Default Constructor we would want for the Move Object.
	Move(int level, string name, string type, string category, int attack, int accuracy, int pp, int effect_percent, string description);

};