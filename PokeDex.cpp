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
#include <algorithm>
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

// initializeLevelData() - Current Version 0.1.0
//
// Implemented to help facilitate the storage of PoGo's leveling data

void PokeDex::initializeLevelData() {
	ifstream levelDataFile("leveldata.json");
	stringstream jsonSS; 
	
	if (levelDataFile) {
		jsonSS << levelDataFile.rdbuf();
		levelDataFile.close();
	}
	else {
		throw std::runtime_error("!! Unable to open json file");
	}

	// Convert the file into a document object in conjunction with rapidJSON
	Document levelDataDoc;

	if (levelDataDoc.Parse<0>(jsonSS.str().c_str()).HasParseError())
		throw std::invalid_argument("JSON parse error");

	// Let rapidJSON know that there's an array name pokemons within the json
	if (!levelDataDoc.IsArray()) {
		levelDataDoc.IsArray();
	}

	const Value& levels = levelDataDoc;
	assert(levels.IsArray());

	// Create a thread pool with 15 worker threads
	ThreadPool pool(8); // 15 Workers provide the best performance
	//auto start = chrono::high_resolution_clock::now();

	for (SizeType i = 0; i < levels.Size(); i++) {
		const Value& level = levels[i];

		pool.AddJob([&](void) { // http://stackoverflow.com/questions/26903602/an-enclosing-function-local-variable-cannot-be-referenced-in-a-lambda-body-unles
			iLDThreadTask(level);
		}
		);
	}

	pool.WaitAll();
	//auto finish = chrono::high_resolution_clock::now();

	//std::cout << chrono::duration_cast<chrono::nanoseconds>(finish - start).count() / 1000000 << " ms" << endl;
}

// initializeLevelData ThreadTask Function
void PokeDex::iLDThreadTask(const rapidjson::Value& level) {
	// Create the LevelData Object
	LevelData currentLevelData(level["level"].GetInt(), level["dust"].GetInt(), level["candy"].GetInt(), level["cpScalar"].GetFloat());
	lock_guard<mutex> lock(initialMutex_);
	levelsData_.push_back(currentLevelData);
}

// initializePokemons() - Current Version 2.3.1  
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
	ThreadPool pool(8); // 15 Workers provide the best performance
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

/* savePokemons() Current Version: 2.1

	Version 2.0
	- Multi Threading Support with ThreadPool

	This has reduced write times by 3X.
	From 27-28.800+ seconds, to 9166ms (9.166 seconds)

	Version 2.1
	- Indices-based for loops
	- Reduction in redundant .at functions
	- Removed Multi Threading Support with ThreadPool to quadruple throughput.
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

	document_->SetArray(); // It's an object
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
	// Therefore, it is theoretically faster than utitlizing the ThreadPool Library to offload
	// the Pokemons.json.
	for (std::vector<Pokemon>::size_type i = 0; i < Pokemons_.size(); i++)
	{
		sPThreadTask(Pokemons_[i]);
	}

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
	//lock_guard<mutex> lock(exitingMutex_);
	// Push the GenericObject to the Document Array
	(*document_).PushBack(pokemon, allocator);
}

void PokeDex::launchSearchMenu() {
	std::system("cls"); // Clear the main menu away from the CLI

	int choice;
	std::cout << "============= Search Menu ===============" << endl;
	std::cout << "(1) Search by Pokemon name" << endl;
	std::cout << "(2) Search a Pokemon by one of it's type" << endl;
	std::cout << "(3) Show all Pokemons" << endl;
	std::cout << "" << endl;
	std::cout << "(5) Back to Main Menu" << endl;
	
	vector<Pokemon*> results;
	for (;;) {
		if (std::cin >> choice) {
			switch (choice) {
			case 1:
				for (;;) {
					results = searchWithName();
					if (!results.empty()) {
						launchPokemonResult(*selectPokemonFromResults(results));
						break;
					}
					else {
						std::cout << "No pokemon was found." << endl;
					}
				}
			case 2:
				for (;;) {
					results = searchWithType();
					
					if (!results.empty()) {
						launchPokemonResult(*selectPokemonFromResults(results));
						break;
					}
					else {
						std::cout << "No pokemon was found." << endl;
					}
				}
				break;
			case 3:
				for (auto it = Pokemons_.begin(); it != Pokemons_.end(); it++) {
					results.push_back(&(*it));
				}

				launchPokemonResult(*selectPokemonFromResults(results));

				break;
			case 5:
				std::system("cls");
				launchMenu();
				break;
			default:
				std::cout << "Please enter a valid choice." << endl;
				std::cin.clear();
				// Now you must get rid of the bad input.
				// Personally I would just ignore the rest of the line
				// http://stackoverflow.com/questions/13212043/integer-input-validation-how
				cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				break;
			}
		}
		else {
			std::cout << "Please enter a valid choice." << endl;
			std::cin.clear();

			// Now you must get rid of the bad input.
			// Personally I would just ignore the rest of the line
			cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
	}

}

// Searches for the Pokemon via it's name, and returns a pointer
// Back
vector<Pokemon*> PokeDex::searchWithName() {
	string searchStr;
	vector<Pokemon*> result;

	for (;;) {
		cout << "Please enter the name of the Pokemon " << endl;

		if (std::cin >> searchStr) {
			break;
		}
		else {
			std::cout << "Please try again." << endl;
			std::cin.clear();
		}
	}

	// http://stackoverflow.com/questions/14124395/c-stdvector-search-for-value
	for (Pokemon &p : Pokemons_) {
		if (p.contains(searchStr)) {
			//cout << "Found " << p.getPokemonName() << endl;
			Pokemon* found = &p;
			result.push_back(found);
		}
	}

	return result;
}

// Searches for the Pokemon via it's type and returns a pointer
// back
vector<Pokemon*> PokeDex::searchWithType() {
	int typeChoice;
	vector <const char *> typesVector = Pokemon::getTypesInString();
	vector<Pokemon*> resultVector;

	int counter = 0;
	cout << "Please choose the type you want to search with: " << endl;
	for (const char * c : typesVector) {
		cout << "(" << counter + 1 << ") " << c << endl;
		counter++;
	}

	for (;;) {
		if (std::cin >> typeChoice) {
			if (typeChoice > 0 && typeChoice < counter + 2) { // Limit against the size of the types vector
				break;
			}
		}

		else {
			std::cout << "Please try again." << endl;
			std::cin.clear();
		}
	}

	Pokemon::Type chosen = static_cast<Pokemon::Type>(typeChoice - 1);

	for (Pokemon& p : Pokemons_) {
		vector<Pokemon::Type>& currTypes = p.getTypesVector();
		if (std::find(currTypes.begin(), currTypes.end(), chosen) != currTypes.end()) {
			// Found something
			resultVector.push_back(&p);

			// Debugging Purposes only
			//std::cout << "Found " << p.getPokemonName() << endl;
		}
	}

	return resultVector;
}

Pokemon* PokeDex::getPokemonById(int id) {
	for (Pokemon& p : Pokemons_) {
		if (p.getPokemonId() == id) {
			return &p;
		}
	}
	// Nothing will get through here because we're passing in an id we know.
	
	return &Pokemons_[0]; // Return the first pokemon since we don't have a choice
}

Pokemon* PokeDex::selectPokemonFromResults(vector<Pokemon*> &results) {
	Pokemon* selected = results[0]; // By default, select the first one
	int count = 1;

	if (results.size() > 1) {
		int choice;
		cout << "Please select your choice: " << endl;
		for (Pokemon* p : results) {
			cout << "(" << count << ") " << p->getPokemonName() << endl;
			count++;
		}

		for (;;) {
			if (std::cin >> choice && choice < (results.size() + 1) && choice > 0) {
				selected = results[choice - 1];
				std::cout << results[choice - 1]->getPokemonName() << " has been selected." << endl;
				break;
			}
			else {
				std::cout << "Please enter a valid choice." << endl;
				std::cin.clear();
			}
		}
	}
	else {
		cout << "Only " << selected->getPokemonName() << " was found." << endl;
	}

	return selected;
}

void PokeDex::launchPokemonResult(Pokemon& pokemon) {
	int choice;
	std::system("cls"); // Clear the console again
	string pokemonName = pokemon.getPokemonName();

	std::cout << "Here are " << pokemon.getPokemonName() << "'s information: " << endl;
	std::cout << "Pokemon ID: " << pokemon.getPokemonId() << endl;
	
	// Show the types
	std::cout << "Type/s: ";
	for (string& t : pokemon.typesToString()) {
		cout << t << " ";
	}
	std::cout << endl;

	// Show the Evolutions
	std::cout << "Evolution/s: ";
	if (!pokemon.getEvolutions().size() < 1) {
		std::cout << endl;
		for (Evolution& e : pokemon.getEvolutions()) {
			Pokemon* evolvedPokemon = getPokemonById(e.getPokemonId());
			std::cout << "Evolving to: " << evolvedPokemon->getPokemonName() << endl;
			std::cout << "Requires: " << e.getEvolvingEvent() << endl;
		}
	}
	else {
		std::cout << "No evolution/s." << endl;
	}

	std::cout << endl;

	// Show the Moves
	std::cout << "Move/s: ";
	if (!pokemon.getMoves().size() < 1) {
		for (Move& m : pokemon.getMoves()) {
			std::cout << endl;
			std::cout << "Move Name: " << m.getMoveName() << endl;
			std::cout << "Move Level Requirement: " << m.getMoveLevel() << endl;
			std::cout << "Move Type: " << m.getMoveType() << endl;
			std::cout << "Move Category: " << m.getMoveCategory() << endl;
			std::cout << "Move Attack Damage: " << m.getMoveAttack() << endl;
			std::cout << "Move Accuracy: " << m.getMoveAccuracy() << endl;
			std::cout << "Move PP: " << m.getMovePP() << endl;
			std::cout << "Move Effect Percent: " << m.getMoveEffectPercent() << endl;
			std::cout << "Move Description: " << m.getMoveDescription() << endl;
		}
	}
	else {
		std::cout << "No move/s." << endl;
	}

	std::cout << endl;
	std::cout << "=====================================" << endl;

	std::cout << "What would you like to do?" << endl;
	std::cout << "(1) Edit " << pokemonName << endl;
	std::cout << "(2) Delete " << pokemonName << endl;
	std::cout << "(3) Go back to the main menu" << endl;

	char answer;
	for (;;) {
		if (std::cin >> choice) {
			switch (choice) {
			case 1:
				launchEditPokemon(pokemon);
				break;
			case 2:
				std::cout << "Are you sure you want to delete " << pokemonName << "? Yes = y | No = n" << endl;
				for (;;) {
					if (std::cin >> answer) {
						switch (answer) {
						case 'y':
							// http://stackoverflow.com/questions/39912/how-do-i-remove-an-item-from-a-stl-vector-with-a-certain-value
							Pokemons_.erase(std::remove(Pokemons_.begin(), Pokemons_.end(), pokemon), Pokemons_.end());
							std::system("cls");
							std::cout << pokemonName << " deleted." << endl;
							launchMenu();
							break;
						case 'n':
							launchPokemonResult(pokemon);
							break;
						default:
							std::cout << "Please enter y/n only." << endl;
						}
					}
					else {
						std::cout << "Please enter y/n only." << endl;
					}
				}				
				break;
			case 3:
				std::system("cls");
				launchMenu();
				break;
			default:
				std::cout << "Please enter one of the choices." << endl;
				std::cin.clear();
				break;
			}
		}
		else {
			std::cout << "Please try again." << endl;
			std::cin.clear();
		}
	}
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
		if (std::cin >> evoChoice && (typeCount > -1 && typeCount < 2)) {
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

Evolution PokeDex::createEvolution() {
	Pokemon* result = selectPokemonFromResults(searchWithName());
	int index = result->getPokemonId();
	string event;

	std::cout << "What is the event that would trigger the evolution?" << endl;
	std::cin >> event;

	std::cout << endl;
	std::cout << "Evolution Created." << endl;

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
		if (std::cin >> Level_ && (Level_ < 100 && Level_ >= 0)) {
			break;
		}
		else {
			std::cout << "Please enter a valid level limit. [0 to 100] (0 for no requirement)" << endl;
			std::cin.clear();
			std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
	}

	std::cout << "What's the name of the move?" << endl;
	std::cin >> Name_;

	std::cout << "What's the type of " + Name_ + "?" << endl;
	std::cin >> Type_;

	std::cout << "What's the category of " + Name_ + "?" << endl;
	std::cin >> Category_;

	cout << "What's the attack damage of " + Name_ + "?" << endl;
	for (;;) {
		if (std::cin >> Attack_ && (Attack_ < 999 && Attack_ >= 0)) {
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
		if (std::cin >> Accuracy_ && (Accuracy_ < 100 && Accuracy_ >= 0)) {
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
		if (std::cin >> Pp_ && (Pp_ < 100 && Pp_ >= 0)) {
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
		if (std::cin >> Effect_percent_ && (Effect_percent_ < 100 && Effect_percent_ >= 0)) {
			break;
		}
		else {
			std::cout << "Please enter a valid percentage limit. [0 to 100] (0 -> Moves without effect)" << endl;
			std::cin.clear();
			std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
	}

	std::cout << "Provide a simple/detailed description for " + Name_ + "." << endl;
	std::cin >> Description_;

	return Move(Level_, Name_, Type_, Category_, Attack_, Accuracy_, Pp_, Effect_percent_, Description_);
}

void PokeDex::launchEditPokemon(Pokemon& pokemon) {
	int choice;

	std::cout << "What would you like to edit for " << pokemon.getPokemonName() << "?" << endl;
	std::cout << "(1) Edit Name" << endl;
	std::cout << "(2) Edit Evolutions" << endl;
	std::cout << "(3) Edit Moves" << endl;
	std::cout << "(4) Return to the previous menu" << endl;
	std:: cout << "(5) Return to the main menu" << endl;

	string input;
	for (;;) {
		if (std::cin >> choice) {
			switch (choice) {
			case 1: // Change the name of the current pokemon
				cout << "So what would you like to name it now?" << endl;
				for (;;) {
					if (std::cin >> input) {
						std::system("cls");
						cout << pokemon.getPokemonName() << " is renamed to " << input << endl;
						pokemon.setPokemonName(input);
						launchEditPokemon(pokemon);
						break;
					}
					else {
						cout << "Please enter something." << endl;
					}
				}
				break;
			case 2: // Edit the evolutions of the pokemon
				launchEditEvolutions(pokemon);
				break;
			case 3: // Edit the moves of the pokemon
				launchEditMoves(pokemon.getExactMoves());
				break;
			case 4: // Get back to the selected pokemon menu
				std::system("cls");
				launchPokemonResult(pokemon);
				break;
			case 5: // Get back to the main menu.
				std::system("cls");
				launchMenu();
				break;
			default:
				std::cout << "Please enter a valid choice." << endl;
				std::cin.clear();
				// Now you must get rid of the bad input.
				// Personally I would just ignore the rest of the line
				// http://stackoverflow.com/questions/13212043/integer-input-validation-how
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				break;
			}
		}
		else {
			std::cout << "Please enter a valid choice." << endl;
			std::cin.clear();

			// Now you must get rid of the bad input.
			// Personally I would just ignore the rest of the line
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
	}
}

void PokeDex::launchEditEvolutions(Pokemon& pokemon) {
	vector<Evolution>& evolutions = pokemon.getExactEvolutions();
	int evolutionSize = evolutions.size();
	int count = 1;
	int choice;
	cout << "Here's the evolution/s: " << endl;

	if (evolutionSize > 0) {
		// Print out all the evolution info first
		for (Evolution evolution : evolutions) {
			Pokemon* p = getPokemonById(evolution.getPokemonId());
			cout << endl;
			cout << "============= Evolution " << count << " =============" << endl;
			cout << "====== Select this by typing (" << count << ") ======" << endl;
			cout << "Evolving to: " << p->getPokemonName() << endl;
			cout << "Evolving Event: " << evolution.getEvolvingEvent() << endl;

			count++;
		}
	}
	else {
		std::cout << endl;
		std::cout << "There are no evolutions." << endl;
	}

	std::cout << endl;
	// We'll set all the misc items to a count of above 10, so that we can accomodate
	// a further custom menu for any of the selected evolution that the user wants to make
	// changes to.
	//std::cout << "What would you like to do?" << endl;
	std::cout << "(10) Add a new evolution" << endl;
	std::cout << "(11) Return to the main menu" << endl;

	for (;;) {
		if (std::cin >> choice) {
				if (choice < (evolutionSize + 1)) {
					bool result = launchEditEvolution(evolutions[choice - 1], pokemon);
					break;
				}
				else if (choice > 9 && choice < 12) {
					if (choice == 10) {
						evolutions.push_back(createEvolution());
					}
					else {
						launchMenu();
					}
					break;
				}
				else {
					std::cout << "Please select a valid input." << endl;
					std::cin.clear();

					// Now you must get rid of the bad input.
					// Personally I would just ignore the rest of the line
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				}
		}
		else {
			std::cout << "Please select a valid input." << endl;
			std::cin.clear();

			// Now you must get rid of the bad input.
			// Personally I would just ignore the rest of the line
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
	}

	int finalSelection;
	std::cout << "Would you like to head back to the main menu or head back to the" << endl;
	std::cout << "previous menu?" << endl;
	std::cout << "(1) Main Menu" << endl;
	std::cout << "(2) Back to the previous menu" << endl;

	for (;;) {
		if (std::cin >> finalSelection) {
			switch (finalSelection) {
			case 1:
				launchMenu();
				break;
			case 2:
				launchEditEvolutions(pokemon);
				break;
			default:
				std::cout << "Please select a valid input." << endl;
				std::cin.clear();

				// Now you must get rid of the bad input.
				// Personally I would just ignore the rest of the line
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
 		}
		else {
			std::cout << "Please select a valid input." << endl;
			std::cin.clear();

			// Now you must get rid of the bad input.
			// Personally I would just ignore the rest of the line
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
	}
}

bool PokeDex::launchEditEvolution(Evolution& evolution, Pokemon& pokemon) {
	int choice;
	string input;

	std::system("cls");
	std::cout << endl << endl;
	std::cout << "Here's the evolution information you're editing" << endl;
	std::cout << "Evolving To: " << getPokemonById(evolution.getPokemonId())->getPokemonName() << endl;
	std::cout << "Evolving Event: " << evolution.getEvolvingEvent() << endl;
	std::cout << endl << endl;
	std::cout << "What would you like to do?" << endl;
	std::cout << "(1) Change the Evolving Pokemon" << endl;
	std::cout << "(2) Change the Evolving Event" << endl;
	std::cout << "(3) Return to the previous menu" << endl;
		
	for (;;) {
		if (std::cin >> choice) {
			switch (choice) {
			case 1:
				// The line below will allow the user to search the pokemon by it's name,
				// select the pokemon from the list of results from the user's input and finally
				// extact out the selected pokemon id to be set into the evolution object.
				evolution.setPokemonId(selectPokemonFromResults(searchWithName())->getPokemonId());
				std::cout << getPokemonById(evolution.getPokemonId())->getPokemonName() << " has been set as the evolving pokemon." << endl;
				return true;
				break;
			case 2:
				std::cout << "Please enter the new evolving event below:" << endl;
				for (;;) {
					if (std::cin >> input) {
						evolution.setEvent(input);
						break;
					}
					else {
						std::cout << "Please try again." << endl;
						std::cin.clear();
					}
				}
				break;
			case 3:
				launchEditEvolutions(pokemon);
			default:
				std::cout << "Please select a valid input." << endl;
				std::cin.clear();

				// Now you must get rid of the bad input.
				// Personally I would just ignore the rest of the line
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
		}
		else {
			std::cout << "Please select a valid input." << endl;
			std::cin.clear();

			// Now you must get rid of the bad input.
			// Personally I would just ignore the rest of the line
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
	}

	std::cout << "Left launchEditEvolution()" << endl;

	return true;
}

void PokeDex::launchEditMoves(vector<Move>& moves) {
	int count = 1;
	cout << "Here's the move/s: " << endl;

	// Print out all the move info first
	for (Move m : moves) {
		cout << endl;
		cout << "============= Move " << count << " =============" << endl;
		std::cout << "Move Name: " << m.getMoveName() << endl;
		std::cout << "Move Level Requirement: " << m.getMoveLevel() << endl;
		std::cout << "Move Type: " << m.getMoveType() << endl;
		std::cout << "Move Category: " << m.getMoveCategory() << endl;
		std::cout << "Move Attack Damage: " << m.getMoveAttack() << endl;
		std::cout << "Move Accuracy: " << m.getMoveAccuracy() << endl;
		std::cout << "Move PP: " << m.getMovePP() << endl;
		std::cout << "Move Effect Percent: " << m.getMoveEffectPercent() << endl;
		std::cout << "Move Description: " << m.getMoveDescription() << endl;		
		
		count++;
	}

	cout << endl;


}

void PokeDex::launchDeletePokemon() {

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
		exit(0); // Exit the program
		break;
	default:
		std::cout << "Please try again" << endl;
		break;
	}
}

void PokeDex::launchMenu() {
	string choice = "";

	while (choice == "") {
		std::cout << "============ C++ PokeDex [Developers Release] ============" << endl;
		std::cout << "(1) Search for a pokemon" << endl;
		std::cout << "(2) Create a new pokemon" << endl;
		std::cout << "(3) Compare two pokemons" << endl;
		std::cout << "(4) PokemonGo IV Calculator" << endl;
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
			std::cin.clear();
			choice = "";
			std::system("cls");
			std::cout << "Invalid Input, Please try again." << endl;
		}
	}
}

int PokeDex::getBytesOfPokemons() const {
	return Pokemons_.size();
}