#include <iostream>

#include "headers/parser.hpp"

// TODO: Destroy scopes when they exit
// TODO: Add errors
// TODO: Comment code
// TODO: Add functions

int main() {
	try {
		stone_lang::exec_program("../../test-code/simple.st");
	} catch (std::exception e) {
		std::cout << "\n=== " << e.what() << " ===\n";
	}

	return 0;
}
