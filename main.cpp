/*
http://stackoverflow.com/questions/5333374/why-am-i-getting-these-already-defined-linker-errors

You should include "VectorDouble.h" and not "VectorDouble.cpp" in Main.cpp.

The whole concept of include files is rather broken in C++ as compared to many other languages.

First C++ divides things up into 'declarations' and 'definitions'. You may only ever have one definition of something in a program, 
but as many declarations as you want. In your VectorDouble.cpp file you are defining things, and in VectorDouble.h file you are declaring things.

The #include directive in C++ is dead simple and stupid. When it is encountered the compiler effectively does a simple textual replacement. 
The #include directive is replaced with the contents of the file you are including.

When you #include a file of definitions, that means you effectively define them all right there where you've done the #include.
This is why you shouldn't be including "VectorDouble.cpp". Since you likely also compile that file as a separate file,
you then end up with at least two copies of all the definitions right there.

This whole declaration vs. definition dichotomy gets very confusing when talking about certain kinds of things. For example, 
if a function is declared inline the function body isn't really considered a definition anymore, exactly. This means you can have as 
many copies of a function body that's been declared inline as you want. All that's required is that all of the definitions be identical.

Similarly, declaring a template function is a declaration, even if you include a function body. This is because the declaration results 
in no code being generated, only the template instantiation results in code generation. And that's the real litmus test for deciding if 
something is a declaration or definition. If it results in space being allocated or actual code being produced right then and there, 
then its a definition, otherwise its a declaration.
*/
#include <stdio.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "json.hpp"
#include "Move.h"
#include "Evolution.h"
#include "Pokemon.h"

// Directory Namespace Inclusion
// https://msdn.microsoft.com/en-us/library/hh874694.aspx
using namespace std::tr2::sys;
using namespace std;

// https://github.com/nlohmann/json
using json = nlohmann::json;

vector<Pokemon> Pokemons; // List Of Pokemons

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

		// Index
		int index = jsonContent["index"]; // Since index is integer by default

		string name = jsonContent["name"];

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
				string str_level = removeQuote((*it)["level"]);
				level = stoi(str_level);
			}
			else { // Else, it means that there are no level requirements
				level = 0; // So we'll just set the level to 0
			}

			string name = (*it)["name"];
			
			string type = (*it)["type"];
			
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
			//cout << newMove.getMoveName() << endl;
			
			// Push newMove to the vector
			moves.push_back(newMove);
		}

		// Create the Pokemon Object
		Pokemon currentPokemon(index, name, evolutions, types, moves);
		Pokemons.push_back(currentPokemon);
	}
}

int main() {
	string nice;

	initializePokemons();

	for (Pokemon pokemon : Pokemons) {
		cout << pokemon.getPokemonName() << endl;
	}

	cin >> nice;
	return 0;
}
