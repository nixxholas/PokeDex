#include "PokeDex.h"
#include <stdio.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "json.hpp"

// Directory Namespace Inclusion
// https://msdn.microsoft.com/en-us/library/hh874694.aspx
using namespace std::tr2::sys;
using namespace std;

// https://github.com/nlohmann/json
using json = nlohmann::json;

PokeDex::PokeDex()
{
}


PokeDex::~PokeDex()
{
}

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
void PokeDex::initializePokemons() {
	string line;

	cout << "Initializing PokeDex Data" << endl;

	for (auto& p : ::recursive_directory_iterator("Pokemons")) {
		// For debugging purposes only
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

		vector<string> types;
		// Iterate through the types array to instantiate the Pokemon's types Array
		for (json::iterator it = jsonContent["types"].begin(); it != jsonContent["types"].end(); ++it) {
			// Debugging Types, Has "
			//cout << *it << endl;			
			string str_type = removeQuote(*it);

			types.push_back(str_type);
		}

		vector<Evolution> evolutions;
		for (json::iterator it = jsonContent["evolutions"].begin(); it != jsonContent["evolutions"].end(); ++it) {
			// Debugging for the output
			//cout << *it << endl;

			int pokemon = (*it)["pokemon"];
			string event = (*it)["event"];

			Evolution evolution(pokemon, event);
			evolutions.push_back(evolution);
		}

		vector<Move> moves;
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
			if (is_number(removeQuote((*it)["level"]))) { // Call is_number to check if we can parse it to an int
														  // Debugging for level integer datatype
														  //cout << removeQuote((*it)["level"]) << endl;
				level = stoi(removeQuote((*it)["level"]));
			}
			else { // Else, it means that there are no level requirements
				level = 0; // So we'll just set the level to 0
			}

			// Category string Variable
			//cout << category << endl;

			// Attack Integer Variable
			int attack;
			if (is_number(removeQuote((*it)["attack"]))) { // Call is_number to check if we can parse it to an int
				attack = stoi(removeQuote((*it)["attack"]));
			}
			else { // Else, it means that there is no attack damage
				attack = 0; // So we'll just set the attack to 0
			}

			// Accuracy Integer Variable
			int accuracy;
			if (is_number(removeQuote((*it)["accuracy"]))) { // Call is_number to check if we can parse it to an int
				accuracy = stoi(removeQuote((*it)["accuracy"]));
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
				effect_percent = stoi(removeQuote((*it)["effect_percent"]));
			}
			else { // Else, it means that there are no level requirements
				effect_percent = 0; // So we'll just set the level to 0
			}

			// (int level, string name, string type, string category,
			//int attack, int accuracy, int pp, int effect_percent, string description)
			Move newMove(level, (*it)["name"], (*it)["type"], (*it)["category"], attack, accuracy, pp, effect_percent, (*it)["description"]);

			// Debugging newMove
			//cout << newMove.getMoveName() << endl;

			// Push newMove to the vector
			moves.push_back(newMove);
		}

		// Create the Pokemon Object
		Pokemon currentPokemon(jsonContent["index"], jsonContent["name"], evolutions, types, moves);
		Pokemons_.push_back(currentPokemon);
	}
}

void PokeDex::menuChoice(int choice) {
	switch (choice) {
	default:
		cout << "Please try again" << endl;
		break;
	}
}