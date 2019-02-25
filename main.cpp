#include "main.h"

using namespace std;

#include <ctype.h>

// // change to take input from query_list.txt 
// int main(){

// 	// create index here
// 	// cout << "Creating index." << endl;
// 	docIndex *index = new docIndex();

// 	// index->print_index(); // testing purposes
// 	cout << "Index created." << endl << endl;


// 	cout << "Enter a term to query: " << endl;

// 	string input = "";
// 	cin >> input; transform(input.begin(), input.end(), input.begin(), ::tolower);

// 	while (input != "quit"){
// 		// test user input here
// 		Porter2Stemmer::trim(input);
// 		Porter2Stemmer::stem(input);

// 		index->test(input);

// 		cout << endl;
// 		cout << "Enter a term to query: " << endl;
// 		cin >> input; transform(input.begin(), input.end(), input.begin(), ::tolower);
// 	}

// 	return 0;
// }