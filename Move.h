#pragma once
#include <string>

using namespace std;

class Move {
private:
	int Level; // Level requirement for the move, can be 0
	string Name; // Name of the move
	string Type; // Type of the move (e.g. Ground)
	string Category; // Category of move, Other, physical, etc.
	int Attack; // Damage of the move, can be 0
	int Accuracy; // Accuracy of the move, max 100
	int Pp; // Quantity for the move
	int Effect_percent; // Effect of the move, can be 0
	string Description; // Description of the move

public:
	Move::Move(int level, string name, string type, string category,
		int attack, int accuracy, int pp, int effect_percent, string description);

	Move::~Move();

	string getMoveName() const;
};