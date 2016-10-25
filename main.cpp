#include <iostream>
#include "Move.h"
#include "Evolution.h"
#include "Pokemon.h"
#include "PokeDex.h"

using namespace std;

int main() {
	cout << "Loading..." << endl;

	PokeDex pokeDex;
	pokeDex.initializePokemons();

	cout << "Loaded. Let's Begin! \n" << endl;

	string nice;

	cin >> nice;
	return 0;
}
