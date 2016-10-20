#include "Move.h"

using namespace std;

Move::Move(int level, string name, string type, string category, int attack, int accuracy,
		int pp, int effect_percent, string description) {
	Level = level;
	Name = name;
	Type = type;
	Category = category;
	Attack = attack;
	Accuracy = accuracy;
	Pp = pp;
	Effect_percent = effect_percent;
	Description = description;
}

string Move::getMoveName() const {
	return this->Name;
}