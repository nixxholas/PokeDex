#include "Grade.h"

Grade::Grade(char gradeChar, double min) :
	gradeChar_(gradeChar),
	min_(min) 
{}

double Grade::GetGradeMin() const {
	return min_;
}

char Grade::GetGradeChar() const {
	return gradeChar_;
}

Grade::~Grade() {};