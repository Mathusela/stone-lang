// Function
int add(int x, int y) {
	return (x + y);
}

int globalVar = (1 + 2) + (3 + 4);	// Some variable
int nestedVar = (1 + (2 + 3));
int test=1+2;
globalVar;
nestedVar;
test;

string var = "HELLO WORLD";
var;

// Main entry point
none main() {
	int localVarWith$Escaped\Chars = 5;
	println("Hello World!");
	scope nestedScope {
		int nestedScope = 0;
	}
}

int func(int a) {
	int a%variable = 10;
}