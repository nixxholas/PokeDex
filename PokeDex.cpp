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
#include "rapidjson/reader.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <iterator>
#include "ThreadPool.h"

// Directory Namespace Inclusion
// https://msdn.microsoft.com/en-us/library/hh874694.aspx
//using namespace std::tr2::sys;
using namespace rapidjson;

// https://github.com/nlohmann/json
//using json = nlohmann::json;

mutex initialMutex_;
mutex exitingMutex_;

PokeDex::PokeDex()
{
	//document_ = new Document();
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

// initializePokemons() Current Version 2.3.1  
//
// 2.3 Update - Utilizing Threads
// With the use of a custom ThreadPool object, we're able
// to emulate Java's ThreadPooling which allows parallelizing
// tasks in order to drastically increase performance.
// 
// Version Performances
//	v1.X -> 14+ seconds
//	v2.X before 2.3 -> 2 - 3 seconds
//	Current -> Around 1100 ms
//
// 2.3.1 Update - Further Optimization
//
// Changes
// -> Types statically casted from integer
// -> Indices-based For loop Iteration for vectors when initializing

void PokeDex::initializePokemons() {
	// Open the Pokemons.json array
	// http://www.cplusplus.com/doc/tutorial/files/
	ifstream pokemonsFile("pokemons.json");
	stringstream jsonSS;

	if (pokemonsFile) {
		jsonSS << pokemonsFile.rdbuf();
		pokemonsFile.close();
	}
	else {
		throw std::runtime_error("!! Unable to open json file");
	}

	// Convert the file into a document object in conjunction with rapidJSON
	Document pokemonDoc;

	if (pokemonDoc.Parse<0>(jsonSS.str().c_str()).HasParseError())
		throw std::invalid_argument("JSON parse error");

	// Let rapidJSON know that there's an array name pokemons within the json
	if (!pokemonDoc.IsArray()) {
		pokemonDoc.IsArray();
	}

	const Value& pokemons = pokemonDoc;
	assert(pokemons.IsArray());

	// Create a thread pool with 15 worker threads
	ThreadPool pool(15); // 15 Workers provide the best performance
	auto start = chrono::high_resolution_clock::now();

	for (SizeType i = 0; i < pokemons.Size(); i++) {
		const Value& pokemon = pokemons[i];

		pool.AddJob([&](void) { // http://stackoverflow.com/questions/26903602/an-enclosing-function-local-variable-cannot-be-referenced-in-a-lambda-body-unles
			iPThreadTask(pokemon);
		}
		);
	}

	pool.WaitAll();
	auto finish = chrono::high_resolution_clock::now();

	system("cls"); // Clear the Console

	std::cout << chrono::duration_cast<chrono::nanoseconds>(finish - start).count() / 1000000 << " ms" << endl;

}

// initializePokemonsThreadTask Function
void PokeDex::iPThreadTask(const Value& pokemon) {
	// ============== Data Seeding Per Pokemon =============== //
	//cout << pokemon["name"].GetString() << endl;

	// Evolution
	vector<Evolution> evolutions;
	for (std::vector<Evolution>::size_type j = 0; j != pokemon["evolutions"].Size(); j++) {
		evolutions.push_back(Evolution(pokemon["evolutions"][j]["pokemon"].GetInt(), pokemon["evolutions"][j]["event"].GetString()));
	}

	// Types
	vector<Pokemon::Type> types;
	// http://discuss.cocos2d-x.org/t/how-to-get-array-inside-json-into-vector/25614/2
	for (std::vector<Pokemon::Type>::size_type k = 0; k < pokemon["types"].Size(); k++) {
		types.push_back(static_cast<Pokemon::Type>((pokemon["types"][k].GetInt())));
	}

	// Moves
	vector<Move> moves;
	for (std::vector<Move>::size_type l = 0; l < pokemon["moves"].Size(); l++) {
		moves.push_back(Move(
			pokemon["moves"][l]["level"].GetInt(), // Level
			pokemon["moves"][l]["name"].GetString(), // Move Name
			pokemon["moves"][l]["type"].GetString(), // Move Type
			pokemon["moves"][l]["category"].GetString(), // Move Category
			pokemon["moves"][l]["attack"].GetInt(), // Attack Damage
			pokemon["moves"][l]["accuracy"].GetInt(), // Move Accuracy Percentage
			pokemon["moves"][l]["pp"].GetInt(), // PP Move Amount
			pokemon["moves"][l]["effect_percent"].GetInt(), // Effect Chance
			pokemon["moves"][l]["description"].GetString())); // Description
	}

	// Create the Pokemon Object
	Pokemon currentPokemon(pokemon["index"].GetInt(), pokemon["name"].GetString(), evolutions, types, moves);
	lock_guard<mutex> lock(initialMutex_);
	Pokemons_.push_back(currentPokemon);
}

// initializePokemons() version 2.2 = Utilizing stringstreams
// This iteration of initializePokemons() is more streamlined with predefined variables

//void PokeDex::initializePokemons() {
//	// Open the Pokemons.json array
//	// http://www.cplusplus.com/doc/tutorial/files/
//	ifstream pokemonsFile("pokemons.json");
//	stringstream jsonSS;
//
//	if (pokemonsFile) {
//		jsonSS << pokemonsFile.rdbuf();
//		pokemonsFile.close();
//	}
//	else {
//		throw std::runtime_error("!! Unable to open json file");
//	}
//
//	// Convert the file into a document object in conjunction with rapidJSON
//	Document pokemonDoc;
//
//	if (pokemonDoc.Parse<0>(jsonSS.str().c_str()).HasParseError())
//		throw std::invalid_argument("json parse error");
//
//	// Let rapidJSON know that there's an array name pokemons within the json
//	if (!pokemonDoc.IsArray()) {
//		pokemonDoc.IsArray();
//	}
//
//	const Value& pokemons = pokemonDoc;
//	assert(pokemons.IsArray());
//
//	auto startTimer = std::chrono::high_resolution_clock::now();
//
//	for (SizeType i = 0; i < pokemons.Size(); i++) {
//		// Debugging Purposes Only
//		//cout << i << endl;
//
//		// ============== Data Seeding Per Pokemon =============== //
//
//		// Evolution
//		vector<Evolution> evolutions;
//		for (SizeType j = 0; j < pokemons[i]["evolutions"].Size(); j++) {
//			// Parse the current evolution object into an auto
//			auto& currObj = pokemons[i]["evolutions"][j];
//
//			// Push the auto object to an evolution object into the vector
//			evolutions.push_back(Evolution(pokemons[i]["evolutions"][j]["pokemon"].GetInt(), pokemons[i]["evolutions"][j]["event"].GetString()));
//		}
//
//		// Types
//		vector<string> types_;
//		// http://discuss.cocos2d-x.org/t/how-to-get-array-inside-json-into-vector/25614/2
//		for (SizeType k = 0; k < pokemons[i]["types"].Size(); k++) {
//			types_.push_back(pokemons[i]["types"][k].GetString());
//		}
//		// Get the proper types in
//		vector<Pokemon::Type> types = Pokemon::stringToTypes(types_);
//
//		// Moves
//		vector<Move> moves;
//		for (SizeType l = 0; l < pokemons[i]["moves"].Size(); l++) {
//			auto& currObj = pokemons[i]["moves"][l];
//
//			moves.push_back(Move(
//				pokemons[i]["moves"][l]["level"].GetInt(), // Level
//				pokemons[i]["moves"][l]["name"].GetString(), // Move Name
//				pokemons[i]["moves"][l]["type"].GetString(), // Move Type
//				pokemons[i]["moves"][l]["category"].GetString(), // Move Category
//				pokemons[i]["moves"][l]["attack"].GetInt(), // Attack Damage
//				pokemons[i]["moves"][l]["accuracy"].GetInt(), // Move Accuracy Percentage
//				pokemons[i]["moves"][l]["pp"].GetInt(), // PP Move Amount
//				pokemons[i]["moves"][l]["effect_percent"].GetInt(), // Effect Chance
//				pokemons[i]["moves"][l]["description"].GetString())); // Description
//		}
//
//		// Create the Pokemon Object
//		Pokemon currentPokemon(pokemons[i]["index"].GetInt(), pokemons[i]["name"].GetString(), evolutions, types, moves);
//		Pokemons_.push_back(currentPokemon);
//		std::cout << ".";
//	}
//
//	cout << endl;
//	auto finishTimer = std::chrono::high_resolution_clock::now();
//	std::cout << Pokemons_.size() + " Pokemons loaded in " << std::chrono::duration_cast<std::chrono::milliseconds>(finishTimer - startTimer).count()<< "ms" << endl;
//
//}

// initializePokemons() version 2.1 = Utilizing stringstreams
// This iteration of initializePokemons() is more streamlined.
// Instead of running an IOStream Wrapper which is a little slower
// than a other streams, we'll use StringStream, the fastest one available
// for rapidJSON

//void PokeDex::initializePokemons() {
//	// Open the Pokemons.json array
//	// http://www.cplusplus.com/doc/tutorial/files/
//	ifstream pokemonsFile("pokemons.json");
//	stringstream jsonSS;
//
//	if (pokemonsFile) {
//		jsonSS << pokemonsFile.rdbuf();
//		pokemonsFile.close();
//	}
//	else {
//		throw std::runtime_error("!! Unable to open json file");
//	}
//
//	// Convert the file into a document object in conjunction with rapidJSON
//	Document pokemonDoc;
//
//	if (pokemonDoc.Parse<0>(jsonSS.str().c_str()).HasParseError())
//		throw std::invalid_argument("json parse error");
//
//	// Let rapidJSON know that there's an array name pokemons within the json
//	if (!pokemonDoc.IsArray()) {
//		pokemonDoc.IsArray();
//	}
//
//	const Value& pokemons = pokemonDoc;
//	assert(pokemons.IsArray());
//
//	for (SizeType i = 0; i < pokemons.Size(); i++) {
//		// Debugging Purposes Only
//		//cout << i << endl;
//
//		// ============== Data Seeding Per Pokemon =============== //
//
//		// Evolution
//		vector<Evolution> evolutions;
//		for (SizeType j = 0; j < pokemons[i]["evolutions"].Size(); j++) {
//			// Parse the current evolution object into an auto
//			auto& currObj = pokemons[i]["evolutions"][j];
//
//			// Push the auto object to an evolution object into the vector
//			evolutions.push_back(Evolution(pokemons[i]["evolutions"][j]["pokemon"].GetInt(), pokemons[i]["evolutions"][j]["event"].GetString()));
//		}
//
//		// Types
//		vector<string> types_;
//		// http://discuss.cocos2d-x.org/t/how-to-get-array-inside-json-into-vector/25614/2
//		for (SizeType k = 0; k < pokemons[i]["types"].Size(); k++) {
//			types_.push_back(pokemons[i]["types"][k].GetString());
//		}
//		// Get the proper types in
//		vector<Pokemon::Type> types = Pokemon::stringToTypes(types_);
//
//		// Moves
//		vector<Move> moves;
//		for (SizeType l = 0; l < pokemons[i]["moves"].Size(); l++) {
//			auto& currObj = pokemons[i]["moves"][l];
//
//			// Level Integer Variable
//			int level;
//			if (is_number(pokemons[i]["moves"][l]["level"].GetString())) { // Call is_number to check if we can parse it to an int
//
//																		   // pokemons[i]["moves"][l]["level"].GetInt()); // Doesn't work
//				level = stoi(pokemons[i]["moves"][l]["level"].GetString());
//			}
//			else { // Else, it means that there are no level requirements
//				level = 0; // So we'll just set the level to 0
//			}
//
//			// Attack Integer Variable
//			int attack;
//			if (is_number(pokemons[i]["moves"][l]["attack"].GetString())) { // Call is_number to check if we can parse it to an int
//
//				attack = stoi(pokemons[i]["moves"][l]["attack"].GetString());
//			}
//			else { // Else, it means that there is no attack damage
//				attack = 0; // So we'll just set the attack to 0
//			}
//
//			// Accuracy Integer Variable
//			int accuracy;
//			if (is_number(pokemons[i]["moves"][l]["accuracy"].GetString())) { // Call is_number to check if we can parse it to an int
//
//				accuracy = stoi(pokemons[i]["moves"][l]["accuracy"].GetString());
//				if (accuracy > 100 || accuracy < 0) { // Don't cheat you bastard
//					accuracy = 0;
//				}
//			}
//			else { // Else, it means that there is no accuracy requirement
//				accuracy = 0; // So we'll just set the accuracy to 0
//			}
//
//			// PP Integer Variable
//			int pp;
//			if (is_number(pokemons[i]["moves"][l]["pp"].GetString())) { // Call is_number to check if we can parse it to an int
//
//				pp = stoi(pokemons[i]["moves"][l]["pp"].GetString());
//			}
//			else { // Else, it means that there is no pp requirement
//				pp = 0; // So we'll just set the pp to 0
//			}
//
//			// Effect Percent Integer Variable
//			int effect_percent;
//			if (is_number(pokemons[i]["moves"][l]["effect_percent"].GetString())) { // Call is_number to check if we can parse it to an int
//
//				effect_percent = stoi(pokemons[i]["moves"][l]["effect_percent"].GetString());
//			}
//			else { // Else, it means that there is no pp requirement
//				effect_percent = 0; // So we'll just set the pp to 0
//			}
//
//			/*Move::Move(int level, string name, string type, string category,
//			int attack, int accuracy, int pp, int effect_percent, string description) :*/
//			moves.push_back(Move(level, // Level
//				pokemons[i]["moves"][l]["name"].GetString(), // Move Name
//				pokemons[i]["moves"][l]["type"].GetString(), // Move Type
//				pokemons[i]["moves"][l]["category"].GetString(), // Move Category
//				attack, // Attack Damage
//				accuracy, // Move Accuracy Percentage
//				pp, // PP Move Amount
//				effect_percent, // Effect Chance
//				pokemons[i]["moves"][l]["description"].GetString())); // Description
//		}
//
//		// Create the Pokemon Object
//		Pokemon currentPokemon(pokemons[i]["index"].GetInt(), pokemons[i]["name"].GetString(), evolutions, types, moves);
//		Pokemons_.push_back(currentPokemon);
//		cout << ".";
//	}
//
//}

// initializePokemons() version 2.0 =  Utilizing iostream Wrappers
// NOTE: To validate why 2.1 is better
// ===> "However, please note that the performance will be 
// much lower than the other streams above." Quoted from rapidJSON
//
// Method to iterate through the pokemons.json 
// to create the Pokemon Objects
// From the proof of research via https://github.com/miloyip/nativejson-benchmark,
// rapidJSON is the fastest parser for JSON for C++ and is thus utilized.

//void PokeDex::initializePokemons() {
//	// Open the Pokemons.json array
//	// http://www.cplusplus.com/doc/tutorial/files/
//	ifstream pokemonsFile("pokemons.json");
//	IStreamWrapper isw(pokemonsFile);
//
//	// Convert the file into a document object in conjunction with rapidJSON
//	Document pokemonDoc;
//	pokemonDoc.ParseStream(isw);
//
//	// Let rapidJSON know that there's an array name pokemons within the json
//	if (!pokemonDoc.IsArray()) {
//		pokemonDoc.IsArray();
//	}
//
//	const Value& pokemons = pokemonDoc;
//
//	assert(pokemons.IsArray());
//
//	for (SizeType i = 0; i < pokemons.Size(); i++) {
//		// Debugging Purposes Only
//		//cout << i << endl;
//
//		// ============== Data Seeding Per Pokemon =============== //
//
//		// Evolution
//		vector<Evolution> evolutions;
//		for (SizeType j = 0; j < pokemons[i]["evolutions"].Size(); j++) {
//			// Parse the current evolution object into an auto
//			auto& currObj = pokemons[i]["evolutions"][j];
//
//			// Push the auto object to an evolution object into the vector
//			evolutions.push_back(Evolution(pokemons[i]["evolutions"][j]["pokemon"].GetInt(), pokemons[i]["evolutions"][j]["event"].GetString()));
//		}
//
//		// Types
//		vector<string> types_;
//		// http://discuss.cocos2d-x.org/t/how-to-get-array-inside-json-into-vector/25614/2
//		for (SizeType k = 0; k < pokemons[i]["types"].Size(); k++) {
//			types_.push_back(pokemons[i]["types"][k].GetString());
//		}
//		// Get the proper types in
//		vector<Pokemon::Type> types = Pokemon::stringToTypes(types_);
//
//		// Moves
//		vector<Move> moves;
//		for (SizeType l = 0; l < pokemons[i]["moves"].Size(); l++) {
//			auto& currObj = pokemons[i]["moves"][l];
//
//			// Level Integer Variable
//			int level;
//			if (is_number(pokemons[i]["moves"][l]["level"].GetString())) { // Call is_number to check if we can parse it to an int
//
//				// pokemons[i]["moves"][l]["level"].GetInt()); // Doesn't work
//				level = stoi(pokemons[i]["moves"][l]["level"].GetString());
//			}
//			else { // Else, it means that there are no level requirements
//				level = 0; // So we'll just set the level to 0
//			}
//
//			// Attack Integer Variable
//			int attack;
//			if (is_number(pokemons[i]["moves"][l]["attack"].GetString())) { // Call is_number to check if we can parse it to an int
//
//				attack = stoi(pokemons[i]["moves"][l]["attack"].GetString());
//			}
//			else { // Else, it means that there is no attack damage
//				attack = 0; // So we'll just set the attack to 0
//			}
//			
//			// Accuracy Integer Variable
//			int accuracy;
//			if (is_number(pokemons[i]["moves"][l]["accuracy"].GetString())) { // Call is_number to check if we can parse it to an int
//
//				accuracy = stoi(pokemons[i]["moves"][l]["accuracy"].GetString());
//				if (accuracy > 100 || accuracy < 0) { // Don't cheat you bastard
//					accuracy = 0;
//				}
//			}
//			else { // Else, it means that there is no accuracy requirement
//				accuracy = 0; // So we'll just set the accuracy to 0
//			}
//
//			// PP Integer Variable
//			int pp;
//			if (is_number(pokemons[i]["moves"][l]["pp"].GetString())) { // Call is_number to check if we can parse it to an int
//
//				pp = stoi(pokemons[i]["moves"][l]["pp"].GetString());
//			}
//			else { // Else, it means that there is no pp requirement
//				pp = 0; // So we'll just set the pp to 0
//			}
//
//			// Effect Percent Integer Variable
//			int effect_percent;
//			if (is_number(pokemons[i]["moves"][l]["effect_percent"].GetString())) { // Call is_number to check if we can parse it to an int
//
//				effect_percent = stoi(pokemons[i]["moves"][l]["effect_percent"].GetString());
//			}
//			else { // Else, it means that there is no pp requirement
//				effect_percent = 0; // So we'll just set the pp to 0
//			}
//
//			/*Move::Move(int level, string name, string type, string category,
//				int attack, int accuracy, int pp, int effect_percent, string description) :*/
//			moves.push_back(Move(level, // Level
//				pokemons[i]["moves"][l]["name"].GetString(), // Move Name
//				pokemons[i]["moves"][l]["type"].GetString(), // Move Type
//				pokemons[i]["moves"][l]["category"].GetString(), // Move Category
//				attack, // Attack Damage
//				accuracy, // Move Accuracy Percentage
//				pp, // PP Move Amount
//				effect_percent, // Effect Chance
//				pokemons[i]["moves"][l]["description"].GetString())); // Description
//		}
//
//		// Create the Pokemon Object
//		Pokemon currentPokemon(pokemons[i]["index"].GetInt(), pokemons[i]["name"].GetString(), evolutions, types, moves);
//		Pokemons_.push_back(currentPokemon);
//		cout << ".";
//	}
//
//}

// initializePokemons() version 1.0 = Utilizing nholmann's JSON
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

/* savePokemons() Current Version: 2.1

	Version 2.0
	- Multi Threading Support with ThreadPool

	This has reduced write times by 3X.
	From 27-28.800+ seconds, to 9166ms (9.166 seconds)

	Version 2.1
	- Indices-based for loops
	- Reduction in redundant .at functions
*/
void PokeDex::savePokemons() {
	std::cout << "Saving your data before exiting.." << endl;

	// Delete the existing JSON
	// http://www.cplusplus.com/reference/cstdio/remove/
	if (remove("pokemons.json") != 0) {
		perror("Error deleting file");
	}
	else {
		puts("File successfully deleted");
	}

	std::cout << endl;
	std::cout << "Saving the new and existing data..";

	// Combine the unstaged and staged vectors
	// http://discuss.cocos2d-x.org/t/solved-how-to-write-json-array-using-rapidjson/29551
	stringstream jsonSS; // Define the stringstream
	//Document outputDocument; // Define the document to receive the C++ Objects for jsonSS

	(*document_).SetArray(); // It's an object
							   //assert(outputDocument.IsArray());

	// For Loop Timer
	auto start = chrono::high_resolution_clock::now();

	// The Single Threaded, Indices-based Pokemon Output Iterator.
	//
	// Push the Existing Pokemons Back
	// Test 1 -> 216ms
	// Test 2 -> 207ms
	// Test 3 -> 230ms
	// Test 4 -> 215ms
	// Test 5 -> 218ms
	// Theoretical Average = 217.2ms
	for (std::vector<Pokemon>::size_type i = 0; i < Pokemons_.size(); i++)
	{
		//Pokemon& pokemon = Pokemons_[i]; // Pokemons_[i] is faster than .at(i)

		//pool.AddJob([&](void) { // http://stackoverflow.com/questions/26903602/an-enclosing-function-local-variable-cannot-be-referenced-in-a-lambda-body-unles
		sPThreadTask(Pokemons_[i]);
		//	}
		//);
	}

	//ThreadPool pool(15); // 15 Workers have shown a 1000ms latency decrease compared to 4 Workers

	// Push the existing pokemons back
	// test 1 -> 7597ms
	// test 2 -> 7633ms
	// test 3 -> 8235ms
	// theoretical average = 7821.6ms
	//for (std::vector<pokemon>::size_type i = 0; i < pokemons_.size(); i++)
	//{
	//	pokemon& pokemon = pokemons_[i]; // pokemons_[i] is faster than .at(i)

	//	pool.addjob([&](void) { // http://stackoverflow.com/questions/26903602/an-enclosing-function-local-variable-cannot-be-referenced-in-a-lambda-body-unles
	//			spthreadtask(pokemons_[i]);
	//		}
	//	);
	//}

	// 7970ms -> Test 1
	// 8660ms -> Test 2, Relaunching after Test 2 crashes the program.
	// 7614ms -> Test 3
	// Theoretical Average = 8081.3ms
	//for (auto const& pokemon : Pokemons_) {
	//	pool.AddJob([&](void) { // http://stackoverflow.com/questions/26903602/an-enclosing-function-local-variable-cannot-be-referenced-in-a-lambda-body-unles
	//		sPThreadTask(pokemon);
	//		}
	//	);
	//}

	//pool.WaitAll();
	auto finish = chrono::high_resolution_clock::now();

	//system("cls");
	std::cout << endl;
	std::cout << chrono::duration_cast<chrono::nanoseconds>(finish - start).count() / 1000000 << " ms" << endl;
	std::cout << "Writing to json" << endl;

	// Output the JSON
	StringBuffer strbuf;
	Writer<StringBuffer> writer(strbuf);
	(*document_).Accept(writer);

	// Output to the file
	ofstream ofs("pokemons.json");
	ofs << strbuf.GetString();
	std::cout << "Done!" << endl;
}

void PokeDex::sPThreadTask(const Pokemon& pokemonObj) {
	Value pokemon(kObjectType);
	Value evolutions(kArrayType);
	Value types(kArrayType);
	Value moves(kArrayType);
	Value reUsable;

	// Debugging Purposes
	//cout << pokemonObj.getPokemonName() << endl;

	Document::AllocatorType& allocator = document_->GetAllocator();

	// Push in the object properties into the GenericObject
	pokemon.AddMember("index", pokemonObj.getPokemonId(), allocator);

	// http://stackoverflow.com/questions/7352099/stdstring-to-char
	// reUsable.SetString(pokemonObj.getPokemonName().c_str()
	pokemon.AddMember("name", reUsable.SetString(pokemonObj.getPokemonName().c_str(), allocator), allocator);

	// Get the types
	for (int j = 0; j < pokemonObj.getTypesSize(); j++) {
		// Previously, the types vector -> json array parsing was done by the conventional
		// enum to string to char method, which undoubtedly proves to show significant decrease in
		// data output performance. 
		// With the introduction of this enum -> int method, we're basically reducing 1 call,
		// ultimately reducing the data parsing latency.
		//
		// Before: 9000 - 10000 ms
		// After: 7677ms - 7927ms
		//
		// Previously, the line below was used to get the integer
		// types.PushBack(reUsable.SetInt(pokemonObj.getTypesVector()[j]), allocator);
		// This call requires a placeholder object, calling a integer setter, and getting the vector element.
		// By using the modified code below, we trimmed out 2 extra calls.
		types.PushBack(pokemonObj.getTypesVector()[j], allocator);
	}
	pokemon.AddMember("types", types, allocator);

	// Get the evolutions
	for (int k = 0; k < pokemonObj.getEvolutions().size(); k++) {
		Value evolution(kObjectType);
		evolution.AddMember("pokemon", pokemonObj.getEvolutions()[k].getPokemonId(), allocator);
		evolution.AddMember("event", reUsable.SetString(pokemonObj.getEvolutions()[k].getEvolvingEvent().c_str(), allocator), allocator);
		evolutions.PushBack(evolution, allocator);
	}
	pokemon.AddMember("evolutions", evolutions, allocator);

	// Get the moves
	for (int l = 0; l < pokemonObj.getMoves().size(); l++) {
		Value move(kObjectType);

		// Set the values for the current move
		move.AddMember("level", pokemonObj.getMoves()[l].getMoveLevel(), allocator);
		move.AddMember("name", reUsable.SetString(pokemonObj.getMoves()[l].getMoveName().c_str(), allocator), allocator);
		move.AddMember("type", reUsable.SetString(pokemonObj.getMoves()[l].getMoveType().c_str(), allocator), allocator);
		move.AddMember("category", reUsable.SetString(pokemonObj.getMoves()[l].getMoveCategory().c_str(), allocator), allocator);
		move.AddMember("attack", pokemonObj.getMoves()[l].getMoveAttack(), allocator);
		move.AddMember("accuracy", pokemonObj.getMoves()[l].getMoveAccuracy(), allocator);
		move.AddMember("pp", pokemonObj.getMoves()[l].getMovePP(), allocator);
		move.AddMember("effect_percent", pokemonObj.getMoves()[l].getMoveEffectPercent(), allocator);
		move.AddMember("description", reUsable.SetString(pokemonObj.getMoves()[l].getMoveDescription().c_str(), allocator), allocator);

		moves.PushBack(move, allocator);
	}
	pokemon.AddMember("moves", moves, allocator);

	// Make outputDocument thread 
	lock_guard<mutex> lock(exitingMutex_);
	// Push the GenericObject to the Document Array
	(*document_).PushBack(pokemon, allocator);
}

// savePokemons() version 1.0

//void PokeDex::savePokemons() {
//	std::cout << "Saving your data before exiting.." << endl;
//
//	// Delete the existing JSON
//	// http://www.cplusplus.com/reference/cstdio/remove/
//	if (remove("pokemons.json") != 0) {
//		perror("Error deleting file");
//	}
//	else {
//		puts("File successfully deleted");
//	}
//
//	std::cout << endl;
//	std::cout << "Saving the new and existing data..";
//
//	// Combine the unstaged and staged vectors
//	// http://discuss.cocos2d-x.org/t/solved-how-to-write-json-array-using-rapidjson/29551
//	stringstream jsonSS; // Define the stringstream
//	Document outputDocument; // Define the document to receive the C++ Objects for jsonSS
//	outputDocument.SetArray(); // It's an object
//	//assert(outputDocument.IsArray());
//	Document::AllocatorType& allocator = outputDocument.GetAllocator();
//
//	// For Loop Timer
//	auto startTimer = std::chrono::high_resolution_clock::now();
//
//	// Push the Existing Pokemons Back
//	for (int i = 0; i < Pokemons_.size(); i++)
//	{
//		Value pokemon(kObjectType);
//		Value evolutions(kArrayType);
//		Value types(kArrayType);
//		Value moves(kArrayType);
//		Value reUsable;
//
//		// Push in the object properties into the GenericObject
//		pokemon.AddMember("index", Pokemons_.at(i).getPokemonId(), allocator);
//
//		reUsable.SetString(Pokemons_.at(i).getPokemonName().c_str(), allocator);
//		// http://stackoverflow.com/questions/7352099/stdstring-to-char
//		pokemon.AddMember("name", reUsable, allocator);
//
//		// Get the types
//		for (int j = 0; j < Pokemons_.at(i).typesToString().size(); j++) {
//			//Value type;
//			types.PushBack(reUsable.SetString(Pokemons_.at(i).typesToString().at(j).c_str(), allocator), allocator);
//		}
//		pokemon.AddMember("types", types, allocator);
//
//		// Get the evolutions
//		for (int k = 0; k < Pokemons_.at(i).getEvolutions().size(); k++) {
//			Value evolution(kObjectType);
//			evolution.AddMember("pokemon", Pokemons_.at(i).getEvolutions().at(k).getPokemonId(), allocator);
//			evolution.AddMember("event", reUsable.SetString(Pokemons_.at(i).getEvolutions().at(k).getEvolvingEvent().c_str(), allocator), allocator);
//			evolutions.PushBack(evolution, allocator);
//		}
//		pokemon.AddMember("evolutions", evolutions, allocator);
//
//		// Get the moves
//		for (int l = 0; l < Pokemons_.at(i).getMoves().size(); l++) {
//			Value move(kObjectType);
//
//			// Set the values for the current move
//			move.AddMember("level", Pokemons_.at(i).getMoves().at(l).getMoveLevel(), allocator);
//			move.AddMember("name", reUsable.SetString(Pokemons_.at(i).getMoves().at(l).getMoveName().c_str(), allocator), allocator);
//			move.AddMember("type", reUsable.SetString(Pokemons_.at(i).getMoves().at(l).getMoveType().c_str(), allocator), allocator);
//			move.AddMember("category", reUsable.SetString(Pokemons_.at(i).getMoves().at(l).getMoveCategory().c_str(), allocator), allocator);
//			move.AddMember("attack", Pokemons_.at(i).getMoves().at(l).getMoveAttack(), allocator);
//			move.AddMember("accuracy", Pokemons_.at(i).getMoves().at(l).getMoveAccuracy(), allocator);
//			move.AddMember("pp", Pokemons_.at(i).getMoves().at(l).getMovePP(), allocator);
//			move.AddMember("effect_percent", Pokemons_.at(i).getMoves().at(l).getMoveEffectPercent(), allocator);
//			move.AddMember("description", reUsable.SetString(Pokemons_.at(i).getMoves().at(l).getMoveDescription().c_str(), allocator), allocator);
//
//			moves.PushBack(move, allocator);
//		}
//		pokemon.AddMember("moves", moves, allocator);
//
//		// Push the GenericObject to the Document Array
//		outputDocument.PushBack(pokemon, allocator);
//	}
//	auto finishTimer = std::chrono::high_resolution_clock::now();
//
//	std::system("cls");
//	cout << endl;
//	std::cout << Pokemons_.size() + " Pokemons unloaded in " << std::chrono::duration_cast<std::chrono::milliseconds>(finishTimer - startTimer).count() << "ms" << endl;
//	std::cout << "Writing to json" << endl;
//
//	// Output the JSON
//	StringBuffer strbuf;
//	Writer<StringBuffer> writer(strbuf);
//	outputDocument.Accept(writer);
//
//	// Output to the file
//	ofstream ofs("pokemons.json");
//	ofs << strbuf.GetString();
//}

void PokeDex::launchSearchMenu() {
	string searchString;
	std::cout << "============= Search Menu ===============" << endl;
	std::cout << "(1) Search by Pokemon name" << endl;
	std::cout << "(2) Search a Pokemon by one of it's type" << endl;
	std::cout << "" << endl;
	std::cout << "(5) Back to Main Menu" << endl;
	std::cin >> searchString;
}

// Launches the Create Pokemon Instance.
//
// User Validation Method Credits:
// http://stackoverflow.com/questions/2075898/good-input-validation-loop-using-cin-c
//
void PokeDex::launchCreatePokemon() {
	int index = Pokemons_.size() + 1; // Fixed Identity Key
	string name;
	vector<Pokemon::Type> types;
	vector<Evolution> evolutions;
	vector<Move> moves;

	// Pokemon Name
	std::cout << "Type in the name of the Pokemon: ";
	for (;;) {
		if (std::cin >> name) {
			break;
		}
		else {
			std::cout << "Please enter a valid characters." << endl;
			std::cin.clear();
		}
	}

	int typeCount;
	// Pokemon Type/s
	std::cout << "How many type/s will " + name + " have? [Maximum of 2 Types]" << endl;
	for (;;) {
		if (std::cin >> typeCount && (typeCount < 3 && typeCount > 0)) {
			break;
		}
		else {
			std::cout << "Please enter a valid integer [1 or 2]" << endl;
			std::cin.clear();
			std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
	}

	// Iterate through the number of types entered
	for (int i = 0; i < typeCount; i++) {
		int choice;

		std::cout << "Choose a type: " << endl;
		Pokemon::printEnumChoices(); // Prints out all the choices

		// Input Checking + Data pushing to vector
		for (;;) {
			if (std::cin >> choice && (choice < 19 && choice > 0)) {
				types.push_back(Pokemon::enumIntToType(choice));
				break;
			}
			else {
				std::cout << "Please enter a valid integer [1 to 18]" << endl;
				std::cin.clear();
				std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}
		}
	}

	// Pokemon Evolution/s
	char evoChoice;
	std::cout << "Will " + name + " have any evolution/s?" << endl;
	std::cout << "Yes (1) or No (0)" << endl;
	for (;;) {
		if (std::cin >> evoChoice && (typeCount > 0 && typeCount < 2)) {
			break;
		}
		else {
			std::cout << "Please enter a valid integer [0 or 1]" << endl;
			std::cin.clear();
			std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
	}
	
	if (evoChoice == 1) {
		int evoCount;
		std::cout << "How many evolution variations will " + name + " have?" << endl;
		for (;;) {
			if (std::cin >> evoCount && (evoCount < 5 && evoCount > 0)) {
				break;
			}
			else {
				std::cout << "Please enter a valid integer [1 to 4]" << endl;
				std::cin.clear();
				std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}
		}
		for (int i = 0; i < evoCount; i++) {
			evolutions.push_back(createEvolution());
		}
	}

	// Pokemon Move/s
	int moveCount;
	std::cout << "How many moves will " + name + " have?" << endl;
	for (;;) {
		if (std::cin >> moveCount && (moveCount < 50 || moveCount > 0)) {
			for (int i = 0; i < moveCount; i++) {
				moves.push_back(createMove());
			}
			break;
		}
		else {
			std::cout << "Please enter a valid integer [1 to 50]" << endl;
			std::cin.clear();
			std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
	}
	
	Pokemon newPokemon(index, name, evolutions, types, moves);
	Pokemons_.push_back(newPokemon);
	std::cout << name + " has been created!" << endl;
	std::cin.get();
	launchMenu();
}

void PokeDex::launchDeletePokemon() {

}

Evolution PokeDex::createEvolution() {
	int index = searchAndGetPokemonIndex();
	string event;

	std::cout << "What is the event that would trigger the evolution?" << endl;
	std::cin >> event;

	return Evolution(index, event);
}

Move PokeDex::createMove() {
	int Level_; // Level requirement for the move, can be 0
	string Name_; // Name of the move
	string Type_; // Type of the move (e.g. Ground)
	string Category_; // Category of move, Other, physical, etc.
	int Attack_; // Damage of the move, can be 0
	int Accuracy_; // Accuracy of the move, max 100
	int Pp_; // Quantity for the move
	int Effect_percent_; // Effect of the move, can be 0
	string Description_; // Description of the move

	cout << "What's the level requirement for the move?" << endl;
	for (;;) {
		if (cin >> Level_ && (Level_ < 100 && Level_ >= 0)) {
			break;
		}
		else {
			std::cout << "Please enter a valid level limit. [0 to 100] (0 for no requirement)" << endl;
			std::cin.clear();
			std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
	}

	cout << "What's the name of the move?" << endl;
	cin >> Name_;

	cout << "What's the type of " + Name_ + "?" << endl;
	cin >> Type_;

	cout << "What's the category of " + Name_ + "?" << endl;
	cin >> Category_;

	cout << "What's the attack damage of " + Name_ + "?" << endl;
	for (;;) {
		if (cin >> Attack_ && (Attack_ < 999 && Attack_ >= 0)) {
			break;
		}
		else {
			std::cout << "Please enter a valid pp limit. [0 to 999] (0 for damage-less moves)" << endl;
			std::cin.clear();
			std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
	}

	cout << "What's the accuracy percentage of " + Name_ + "?" << endl;
	for (;;) {
		if (cin >> Accuracy_ && (Accuracy_ < 100 && Accuracy_ >= 0)) {
			break;
		}
		else {
			std::cout << "Please enter a valid percentage limit. [0 to 100] (0 for no requirement)" << endl;
			std::cin.clear();
			std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
	}
	
	cout << "What's the PP of " + Name_ + "?" << endl;
	for (;;) {
		if (cin >> Pp_ && (Pp_ < 100 && Pp_ >= 0)) {
			break;
		}
		else {
			std::cout << "Please enter a valid pp limit. [0 to 100] (0 for no requirement)" << endl;
			std::cin.clear();
			std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
	}

	cout << "What's the effect chance percentage of " + Name_ + "?" << endl;
	for (;;) {
		if (cin >> Effect_percent_ && (Effect_percent_ < 100 && Effect_percent_ >= 0)) {
			break;
		}
		else {
			std::cout << "Please enter a valid percentage limit. [0 to 100] (0 -> Moves without effect)" << endl;
			std::cin.clear();
			std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
	}

	cout << "Provide a simple/detailed description for " + Name_ + "." << endl;
	cin >> Description_;

	return Move(Level_, Name_, Type_, Category_, Attack_, Accuracy_, Pp_, Effect_percent_, Description_);
}

void PokeDex::menuChoice(int& choice) {
	switch (choice) {
	case 1:
		launchSearchMenu();
		break;
	case 2:
		launchCreatePokemon();
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		savePokemons();
		system("pause"); // Windows Command Prompt Compatible only.
		exit(0);
		break;
	default:
		std::cout << "Please try again" << endl;
		break;
	}
}

int PokeDex::searchAndGetPokemonIndex() {
	string search;

	cout << "Enter the name of the pokemon you would like to find: " << endl;
	cin >> search;

	cout << "Searching..." << endl;

	for (Pokemon p : Pokemons_) {
		if (p.getPokemonName() == search) {
			cout << p.getPokemonId() << endl;
			cout << p.getPokemonName() << endl;
			return p.getPokemonId();
		}
	}

	while (true) {
		cout << "Pokemon not found, please search again." << endl;
		cin >> search;
		for (Pokemon p : Pokemons_) {
			if (p.getPokemonName() == search) {
				cout << p.getPokemonId() << endl;
				cout << p.getPokemonName() << endl;
				return p.getPokemonId();
			}
		}
	}

	//auto iter = find_if(Pokemons_.begin(),
	//	Pokemons_.end(), [&search](const Pokemon& obj) { return obj.getPokemonName() == search; });

	//if (iter != Pokemons_.end())
	//{
	//	// found element. it is an iterator to the first matching element.
	//	// if you really need the index, you can also get it:		
	//	cout << distance(Pokemons_.begin(), iter) << endl;
	//	

	//	return distance(Pokemons_.begin(), iter);
	//}

	return 0;
}

void PokeDex::launchMenu() {
	string choice = "";

	while (choice == "") {
		std::cout << "============ C++ PokeDex [Developers Release] ============" << endl;
		std::cout << "(1) Search for a pokemon" << endl;
		std::cout << "(2) Create a new pokemon" << endl;
		std::cout << "(3) Compare two pokemons" << endl;
		std::cout << "(4) Command Mode" << endl;
		std::cout << "(5) Exit the PokeDex" << endl;

		std::cin >> choice; // http://stackoverflow.com/questions/13421965/using-cin-get-to-get-an-integer
		int choiceInt; // Parsing the choice to here later

		if (choice == "1" ||
			choice == "2" ||
			choice == "3" ||
			choice == "4" ||
			choice == "5") {
			choiceInt = stoi(choice);
			menuChoice(choiceInt);
		}
		else {
			cin.clear();
			choice = "";
			std::system("cls");
			std::cout << "Invalid Input, Please try again." << endl;
		}
	}
}

int PokeDex::getBytesOfPokemons() const {
	return Pokemons_.size();
}