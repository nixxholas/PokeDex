#pragma once
#include <string>

using namespace std;

class Move {
private:
	int Level_; // Level requirement for the move, can be 0
	string Name_; // Name of the move
	string Type_; // Type of the move (e.g. Ground)
	string Category_; // Category of move, Other, physical, etc.
	int Attack_; // Damage of the move, can be 0
	int Accuracy_; // Accuracy of the move, max 100
	int Pp_; // Quantity for the move
	int Effect_percent_; // Effect of the move, can be 0
	string Description_; // Description of the move

public:
	Move::Move(int, string, string, string, int, int, int pp, int, string);

	Move::~Move();

	string getMoveName() const;
};