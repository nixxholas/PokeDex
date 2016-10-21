#include <stdio.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "json.hpp"
#include "Move.h"

// Directory Namespace Inclusion
// https://msdn.microsoft.com/en-us/library/hh874694.aspx
using namespace std::tr2::sys;
using namespace std;

// https://github.com/nlohmann/json
using json = nlohmann::json;

// Integer Checker
// http://stackoverflow.com/questions/4917265/can-i-tell-if-a-stdstring-represents-a-number-using-stringstream
bool is_number(const string& s)
{
	if (s.size() == 0) return false;
	for (int i = 0; i<s.size(); i++) {
		if ((s[i] >= '0' && s[i] <= '9') == false) {
			return false;
		}
	}
	return true;
}

string removeQuote(string s) {
	// Remove all double-quote characters
	string result = s;
	result = s.substr(1, s.size() - 2);
	return result;
}

// Method to iterate through the Pokemons folder dynamically
// to create the Pokemon objects
void initializePokemons() {
	string line;
	int i = 0;
	// For debugging purposes only
	//int i = 1;

	for (auto& p : ::recursive_directory_iterator("Pokemons")) {
		// For debugging purposes only
		// Debugging Phase I
		//cout << i << endl;
		//i++;

		// Debugging Phase II
		//cout << p << endl;
		
		// cout is a stream, so basically we're just getting the p to a stream
		// http://stackoverflow.com/questions/3513173/converting-ostream-into-standard-string
		ostringstream stream; // http://www.cplusplus.com/reference/sstream/ostringstream/
		
		// Bring the p to the ostringstream since it is a string when sent to a stream
		stream << p;
		
		// Finally just convert the stream to a string
		string path = stream.str();

		// Current jsonContent
		json jsonContent;

		// Reads the file from the string path
		ifstream in(path);

		// Parse the Stream to the Json Object
		// http://stackoverflow.com/questions/33628250/c-reading-a-json-object-from-file-with-nlohmann-json
		in >> jsonContent;

		// Debugging jsonContent
		//cout << jsonContent["name"] << endl; //  Works

		// Iterate through the moves array to instantiate the Pokemon's Moves Array
		for (json::iterator it = jsonContent["moves"].begin(); it != jsonContent["moves"].end(); ++it) {					
			// Debugging for the output
			// * means De-reference
			//cout << *it << endl; // Works

			/*
				Converting Json Object Attributes to
				C++ based Datatypes
			
				str::stoi
				http://stackoverflow.com/questions/7663709/convert-string-to-int-c

				removeQuote()
				A custom quote remover for strings

				is_number()
				StackOverFlow's response to a string to integer checker
			*/

			// Define the strings for the current Move object
			// So that things will look like it's in an orderly manner
			// Level Integer Variable
			int level;
			if (is_number(removeQuote((*it)["level"]))) { // Call is_number to check if we can pasrse it to an int
				// Debugging for level integer datatype
				//cout << removeQuote((*it)["level"]) << endl;
				string str_level = removeQuote((*it)["level"]);
				level = stoi(str_level);
			}
			else { // Else, it means that there are no level requirements
				level = 0; // So we'll just set the level to 0
			}

			//int level = (*it)["level"];
			string name = (*it)["name"];
			
			string type = (*it)["type"]; // Undone, it's an array
			
			// Category string Variable
			//cout << category << endl;
			string category = (*it)["category"];

			// Attack Integer Variable
			int attack;
			if (is_number(removeQuote((*it)["attack"]))) { // Call is_number to check if we can parse it to an int
				string str_attack = removeQuote((*it)["attack"]);
				attack = stoi(str_attack);
			}
			else { // Else, it means that there is no attack damage
				attack = 0; // So we'll just set the attack to 0
			}

			// Accuracy Integer Variable
			int accuracy;
			if (is_number(removeQuote((*it)["accuracy"]))) { // Call is_number to check if we can parse it to an int
				string str_accuracy = removeQuote((*it)["accuracy"]);
				accuracy = stoi(str_accuracy);
			}
			else { // Else, it means that there are no level requirements
				accuracy = 0; // So we'll just set the level to 0
			}

			// PP Integer Variable
			int pp;
			if (is_number(removeQuote((*it)["pp"]))) { // Call is_number to check if we can parse it to an int
				string str_pp = removeQuote((*it)["pp"]);
				pp = stoi(str_pp);
			}
			else { // Else, it means that there isn't a pp limit
				pp = 0; // So we'll just set the pp to 0
			}

			// Accuracy Integer Variable
			int effect_percent;
			if (is_number(removeQuote((*it)["effect_percent"]))) { // Call is_number to check if we can parse it to an int
				string str_effect_percent = removeQuote((*it)["effect_percent"]);
				effect_percent = stoi(str_effect_percent);
			}
			else { // Else, it means that there are no level requirements
				effect_percent = 0; // So we'll just set the level to 0
			}

			string description = (*it)["description"];

			// (int level, string name, string type, string category,
			//int attack, int accuracy, int pp, int effect_percent, string description)
			Move newMove(level, name, type, category, attack, accuracy, pp, effect_percent, description);
 		
			// Debugging newMove
			cout << newMove.getMoveName() << endl;
		}

		// ------------------------------- DEPRECATED CODE ----------------------------- //

		// Iterate through each line in the ifstream
		// http://stackoverflow.com/questions/13103296/reading-from-file-line-by-line-in-ceach-line-has-one-string-and-several-int-v
		//while (getline(in, line)) {
		//	if (i == 1) { // Index
		//		cout << line << endl;
		//	}
		//	else if (i == 2) { // Name of Pokemon
		//		cout << line << endl;
		//	}
		//	i++;
		//}

		// Read the contents of the file via the ifstream object
		//string contents((istreambuf_iterator<char>(in)),
		//	istreambuf_iterator<char>());
		
		// Print out what is contained within the file.
		//cout << contents << endl;

		// --------------------------- END OF DEPRECATED CODE ------------------------- //

		// Reset the integer
		i = 0;
	}

	/*
	Hard-Coded Code for File Directory Reading

	* Deprecated *
	
	for (size_t i = 1; i <= 10; i++)
	{
		string path;
		if (i < 10) {
			path = "00";
		}
		else if (i < 100) {
			path = "0";
		}

		path = ".\\pokemon\\" + path + to_string(i) + ".json";
		cout << path << endl;

		ifstream in(path);
		string contents((istreambuf_iterator<char>(in)),
			istreambuf_iterator<char>());
		cout << contents << endl;
	}*/
}

int main() {
	string nice;
	//vector<Pokemon> Pokemons;

	initializePokemons();
	cin >> nice;
	return 0;
}
