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
#include "PokeDex.h"
#include <stdio.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include <fstream>
#include <iostream>
#include <filesystem>

// Directory Namespace Inclusion
// https://msdn.microsoft.com/en-us/library/hh874694.aspx
//using namespace std::tr2::sys;
using namespace std;
using namespace rapidjson;

// https://github.com/nlohmann/json
//using json = nlohmann::json;

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
	for (int i = 0; i < s.size(); i++) {
		if ((s[i] >= '0' && s[i] <= '9') == false) {
			return false;
		}
	}
	return true;
}

string removeQuote(string s) {
	// Remove all double-quote characters
	return s.substr(1, s.size() - 2);
}

// Method to iterate through the pokemons.json 
// to create the Pokemon Objects
// From the proof of research via https://github.com/miloyip/nativejson-benchmark,
// rapidJSON is the fastest parser for JSON for C++ and is thus utilized.

void PokeDex::initializePokemons() {
	// Open the Pokemons.json array
	// http://www.cplusplus.com/doc/tutorial/files/
	ifstream pokemonsFile("pokemons.json");
	IStreamWrapper isw(pokemonsFile);

	// Convert the file into a document object in conjunction with rapidJSON
	Document pokemonDoc;
	pokemonDoc.ParseStream(isw);

	// Let rapidJSON know that there's an array name pokemons within the json
	if (!pokemonDoc.IsArray()) {
		pokemonDoc.IsArray();
	}

	const Value& pokemons = pokemonDoc;

	assert(pokemons.IsArray());

	for (SizeType i = 0; i < pokemons.Size(); i++) {
		// Debugging Purposes Only
		//cout << i << endl;

		// ============== Data Seeding Per Pokemon =============== //

		// Evolution
		vector<Evolution> evolutions;
		for (SizeType j = 0; j < pokemons[i]["evolutions"].Size(); j++) {
			// Parse the current evolution object into an auto
			auto& currObj = pokemons[i]["evolutions"][j];

			// Push the auto object to an evolution object into the vector
			evolutions.push_back(Evolution(pokemons[i]["evolutions"][j]["pokemon"].GetInt(), pokemons[i]["evolutions"][j]["event"].GetString()));
		}

		// Types
		vector<string> types_;
		// http://discuss.cocos2d-x.org/t/how-to-get-array-inside-json-into-vector/25614/2
		for (SizeType k = 0; k < pokemons[i]["types"].Size(); k++) {
			types_.push_back(pokemons[i]["types"][k].GetString());
		}
		// Get the proper types in
		vector<Pokemon::Type> types = Pokemon::stringToTypes(types_);

		// Moves
		vector<Move> moves;
		for (SizeType l = 0; l < pokemons[i]["moves"].Size(); l++) {
			auto& currObj = pokemons[i]["moves"][l];

			// Level Integer Variable
			int level;
			if (is_number(pokemons[i]["moves"][l]["level"].GetString())) { // Call is_number to check if we can parse it to an int

				// pokemons[i]["moves"][l]["level"].GetInt()); // Doesn't work
				level = stoi(pokemons[i]["moves"][l]["level"].GetString());
			}
			else { // Else, it means that there are no level requirements
				level = 0; // So we'll just set the level to 0
			}

			// Attack Integer Variable
			int attack;
			if (is_number(pokemons[i]["moves"][l]["attack"].GetString())) { // Call is_number to check if we can parse it to an int

				attack = stoi(pokemons[i]["moves"][l]["attack"].GetString());
			}
			else { // Else, it means that there is no attack damage
				attack = 0; // So we'll just set the attack to 0
			}
			
			// Accuracy Integer Variable
			int accuracy;
			if (is_number(pokemons[i]["moves"][l]["accuracy"].GetString())) { // Call is_number to check if we can parse it to an int

				accuracy = stoi(pokemons[i]["moves"][l]["accuracy"].GetString());
				if (accuracy > 100 || accuracy < 0) { // Don't cheat you bastard
					accuracy = 0;
				}
			}
			else { // Else, it means that there is no accuracy requirement
				accuracy = 0; // So we'll just set the accuracy to 0
			}

			// PP Integer Variable
			int pp;
			if (is_number(pokemons[i]["moves"][l]["pp"].GetString())) { // Call is_number to check if we can parse it to an int

				pp = stoi(pokemons[i]["moves"][l]["pp"].GetString());
			}
			else { // Else, it means that there is no pp requirement
				pp = 0; // So we'll just set the pp to 0
			}

			// Effect Percent Integer Variable
			int effect_percent;
			if (is_number(pokemons[i]["moves"][l]["effect_percent"].GetString())) { // Call is_number to check if we can parse it to an int

				effect_percent = stoi(pokemons[i]["moves"][l]["effect_percent"].GetString());
			}
			else { // Else, it means that there is no pp requirement
				effect_percent = 0; // So we'll just set the pp to 0
			}

			/*Move::Move(int level, string name, string type, string category,
				int attack, int accuracy, int pp, int effect_percent, string description) :*/
			moves.push_back(Move(level, // Level
				pokemons[i]["moves"][l]["name"].GetString(), // Move Name
				pokemons[i]["moves"][l]["type"].GetString(), // Move Type
				pokemons[i]["moves"][l]["category"].GetString(), // Move Category
				attack, // Attack Damage
				accuracy, // Move Accuracy Percentage
				pp, // PP Move Amount
				effect_percent, // Effect Chance
				pokemons[i]["moves"][l]["description"].GetString())); // Description
		}

		// Create the Pokemon Object
		Pokemon currentPokemon(pokemons[i]["index"].GetInt(), pokemons[i]["name"].GetString(), evolutions, types, moves);
		Pokemons_.push_back(currentPokemon);
	}

}

// Method to iterate through the Pokemons folder dynamically
// to create the Pokemon objects
// Time taken: 13.56 secs
//#include "json.hpp" nlohmann's JSON hpp

//void PokeDex::initializePokemons() {
//	string line;
//
//	cout << "Initializing PokeDex Data" << endl;
//
//	for (auto& p : ::recursive_directory_iterator("Pokemons")) {
//		// For debugging purposes only
//		//cout << p << endl;
//
//		// cout is a stream, so basically we're just getting the p to a stream
//		// http://stackoverflow.com/questions/3513173/converting-ostream-into-standard-string
//		ostringstream stream; // http://www.cplusplus.com/reference/sstream/ostringstream/
//
//							  // Bring the p to the ostringstream since it is a string when sent to a stream
//		stream << p;
//
//		// Finally just convert the stream to a string
//		string path = stream.str();
//
//		// Current jsonContent
//		json jsonContent;
//
//		// Reads the file from the string path
//		ifstream in(path);
//
//		// Parse the Stream to the Json Object
//		// http://stackoverflow.com/questions/33628250/c-reading-a-json-object-from-file-with-nlohmann-json
//		in >> jsonContent;
//
//		// Debugging jsonContent
//		//cout << jsonContent["name"] << endl; //  Works
//
//		vector<string> types;
//		// Iterate through the types array to instantiate the Pokemon's types Array
//		for (json::iterator it = jsonContent["types"].begin(); it != jsonContent["types"].end(); ++it) {
//			// Debugging Types, Has "
//			//cout << *it << endl;			
//			string str_type = removeQuote(*it);
//
//			types.push_back(str_type);
//		}
//
//		vector<Evolution> evolutions;
//		for (json::iterator it = jsonContent["evolutions"].begin(); it != jsonContent["evolutions"].end(); ++it) {
//			// Debugging for the output
//			//cout << *it << endl;
//
//			int pokemon = (*it)["pokemon"];
//			string event = (*it)["event"];
//
//			Evolution evolution(pokemon, event);
//			evolutions.push_back(evolution);
//		}
//
//		vector<Move> moves;
//		// Iterate through the moves array to instantiate the Pokemon's Moves Array
//		for (json::iterator it = jsonContent["moves"].begin(); it != jsonContent["moves"].end(); ++it) {
//			// Debugging for the output
//			// * means De-reference
//			//cout << *it << endl; // Works
//
//			/*
//			Converting Json Object Attributes to
//			C++ based Datatypes
//
//			str::stoi
//			http://stackoverflow.com/questions/7663709/convert-string-to-int-c
//
//			removeQuote()
//			A custom quote remover for strings
//
//			is_number()
//			StackOverFlow's response to a string to integer checker
//			*/
//
//			// Define the strings for the current Move object
//			// So that things will look like it's in an orderly manner
//			// Level Integer Variable
//			int level;
//			if (is_number(removeQuote((*it)["level"]))) { // Call is_number to check if we can parse it to an int
//														  // Debugging for level integer datatype
//														  //cout << removeQuote((*it)["level"]) << endl;
//				level = stoi(removeQuote((*it)["level"]));
//			}
//			else { // Else, it means that there are no level requirements
//				level = 0; // So we'll just set the level to 0
//			}
//
//			// Category string Variable
//			//cout << category << endl;
//
//			// Attack Integer Variable
//			int attack;
//			if (is_number(removeQuote((*it)["attack"]))) { // Call is_number to check if we can parse it to an int
//				attack = stoi(removeQuote((*it)["attack"]));
//			}
//			else { // Else, it means that there is no attack damage
//				attack = 0; // So we'll just set the attack to 0
//			}
//
//			// Accuracy Integer Variable
//			int accuracy;
//			if (is_number(removeQuote((*it)["accuracy"]))) { // Call is_number to check if we can parse it to an int
//				accuracy = stoi(removeQuote((*it)["accuracy"]));
//			}
//			else { // Else, it means that there are no level requirements
//				accuracy = 0; // So we'll just set the level to 0
//			}
//
//			// PP Integer Variable
//			int pp;
//			if (is_number(removeQuote((*it)["pp"]))) { // Call is_number to check if we can parse it to an int
//				string str_pp = removeQuote((*it)["pp"]);
//				pp = stoi(str_pp);
//			}
//			else { // Else, it means that there isn't a pp limit
//				pp = 0; // So we'll just set the pp to 0
//			}
//
//			// Accuracy Integer Variable
//			int effect_percent;
//			if (is_number(removeQuote((*it)["effect_percent"]))) { // Call is_number to check if we can parse it to an int
//				effect_percent = stoi(removeQuote((*it)["effect_percent"]));
//			}
//			else { // Else, it means that there are no level requirements
//				effect_percent = 0; // So we'll just set the level to 0
//			}
//
//			// (int level, string name, string type, string category,
//			//int attack, int accuracy, int pp, int effect_percent, string description)
//			Move newMove(level, (*it)["name"], (*it)["type"], (*it)["category"], attack, accuracy, pp, effect_percent, (*it)["description"]);
//
//			// Debugging newMove
//			//cout << newMove.getMoveName() << endl;
//
//			// Push newMove to the vector
//			moves.push_back(newMove);
//		}
//
//		// Create the Pokemon Object
//		Pokemon currentPokemon(jsonContent["index"], jsonContent["name"], evolutions, types, moves);
//		Pokemons_.push_back(currentPokemon);
//	}
//}

void PokeDex::menuChoice(int choice) {
	switch (choice) {
	default:
		cout << "Please try again" << endl;
		break;
	}
}