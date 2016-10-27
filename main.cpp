#include <iostream>
#include "Move.h"
#include "Evolution.h"
#include "Pokemon.h"
#include "PokeDex.h"
#include "conio.h"

using namespace std;

int main() {
	cout << "Loading Pokemons" << endl;

	PokeDex pokeDex;
	pokeDex.initializePokemons();

	cout << "Loaded. Let's Begin!" << endl;
	
	//system("cls"); // Clear the Console

	pokeDex.launchMenu();

	return 0;
}
