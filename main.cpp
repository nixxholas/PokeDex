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

// https://github.com/nlohmann/json
using json = nlohmann::json;

void initializePokemons() {
	std::string line;
	int i = 0;
	// For debugging purposes only
	//int i = 1;

	for (auto& p : ::recursive_directory_iterator("Pokemons")) {
		// For debugging purposes only
		// Debugging Phase I
		//std::cout << i << std::endl;
		//i++;

		// Debugging Phase II
		//std::cout << p << std::endl;
		
		// cout is a stream, so basically we're just getting the p to a stream
		// http://stackoverflow.com/questions/3513173/converting-ostream-into-standard-string
		std::ostringstream stream; // http://www.cplusplus.com/reference/sstream/ostringstream/
		
		// Bring the p to the ostringstream since it is a string when sent to a stream
		stream << p;
		
		// Finally just convert the stream to a string
		std::string path = stream.str();

		// Current jsonContent
		json jsonContent;

		// Reads the file from the string path
		std::ifstream in(path);

		// Parse the Stream to the Json Object
		in >> jsonContent;

		// Debugging jsonContent
		std::cout << jsonContent["name"] << std::endl;


		// ------------------------------- DEPRECATED CODE ----------------------------- //

		// Iterate through each line in the ifstream
		// http://stackoverflow.com/questions/13103296/reading-from-file-line-by-line-in-ceach-line-has-one-string-and-several-int-v
		//while (std::getline(in, line)) {
		//	if (i == 1) { // Index
		//		std::cout << line << std::endl;
		//	}
		//	else if (i == 2) { // Name of Pokemon
		//		std::cout << line << std::endl;
		//	}
		//	i++;
		//}

		// Read the contents of the file via the ifstream object
		//std::string contents((std::istreambuf_iterator<char>(in)),
		//	std::istreambuf_iterator<char>());
		
		// Print out what is contained within the file.
		//std::cout << contents << std::endl;

		// --------------------------- END OF DEPRECATED CODE ------------------------- //

		// Reset the integer
		i = 0;
	}

	/*
	Hard-Coded Code for File Directory Reading

	* Deprecated *
	
	for (size_t i = 1; i <= 10; i++)
	{
		std::string path;
		if (i < 10) {
			path = "00";
		}
		else if (i < 100) {
			path = "0";
		}

		path = ".\\pokemon\\" + path + std::to_string(i) + ".json";
		std::cout << path << std::endl;

		std::ifstream in(path);
		std::string contents((std::istreambuf_iterator<char>(in)),
			std::istreambuf_iterator<char>());
		std::cout << contents << std::endl;
	}*/
}

int main() {
	std::string nice;

	initializePokemons();
	std::cin >> nice;
	return 0;
}
