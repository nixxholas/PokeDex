#pragma once
class Grade {
private:
	char gradeChar_;
	double min;

public:
	// Retrieves the minimum requirement for the grade
	double GetGradeMin() const;

	// Retrieves the current grade character
	char GetGradeChar() const;

	// Constructor
	Grade(char, int);

	// Destructor
	~Grade();
};