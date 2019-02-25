#ifndef MAIN_H
#define MAIN_H

#include "vsm.h"

#include <string>
#include <fstream>
#include <cstdlib>

#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// change to take input from query_list.txt 
int main(){

	// create index here
	// cout << "Creating index." << endl;
	docIndex *index = new docIndex();

	// index->print_index(); // testing purposes
	cout << "Index created." << endl << endl;

	ifstream inputQuery;
	string query, queryTerm;

	inputQuery.open("data/query_list.txt");
	if(!inputQuery.is_open()){
		cout << "Error opening query file." << endl;
		return -1;
	}

	getline(inputQuery,query);	// gets the first line of queries

	int queryNo = stoi(query.substr(0,2));
	query = query.substr(3);

	stringstream query_stream(query);
	while(query_stream >> queryTerm){
		transform(queryTerm.begin(), queryTerm.end(), queryTerm.begin(), ::tolower);

		cout << queryTerm << endl;


	}


	// cout << "Enter a term to query: " << endl;

	// string input = "";
	// cin >> input; transform(input.begin(), input.end(), input.begin(), ::tolower);

	// while (input != "quit"){
	// 	// test user input here
	// 	Porter2Stemmer::trim(input);
	// 	Porter2Stemmer::stem(input);

	// 	index->test(input);

	// 	cout << endl;
	// 	cout << "Enter a term to query: " << endl;
	// 	cin >> input; transform(input.begin(), input.end(), input.begin(), ::tolower);
	// }

	return 0;
}

#endif