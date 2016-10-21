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
