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

string Move::getMoveName() const {
	return Name_;
}