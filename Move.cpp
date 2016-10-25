#include "Move.h"

using namespace std;

Move::Move(int level, string name, string type, string category,
		int attack, int accuracy, int pp, int effect_percent, string description) : 
	Level_(level),
	Name_(name),
	Type_(type),
	Category_(category),
	Attack_(attack),
	Accuracy_(accuracy),
	Pp_(pp),
	Effect_percent_(effect_percent),
	Description_(description)
{ }

Move::~Move() { }

// Returns the level integer
int Move::getMoveLevel() const {
	return Level_;
}

string Move::getMoveName() const {
	return Name_;
}

// Returns the Move type in string
string Move::getMoveType() const {
	return Type_;
}

// Returns the move category in string
string Move::getMoveCategory() const {
	return Category_;
}

// Returns the attack damage in integer
int Move::getMoveAttack() const {
	return Attack_;
}

// Returns the accuracy of the move
int Move::getMoveAccuracy() const {
	return Accuracy_;
}

// Returns the PP amount of the move
int Move::getMovePP() const {
	return Pp_;
}

// Returns the effect percent of the move
int Move::getMoveEffectPercent() const {
	return Effect_percent_;
}

// Returns the description of the move
string Move::getMoveDescription() const {
	return Description_;
}