#include <iostream>

#include "headers/parser.hpp"

// TODO: Destroy scopes when they exit
// TODO: Add errors
// TODO: Comment code

int main() {
	stone_lang::exec_program("../../test-code/simple.st");

	return 0;
}
