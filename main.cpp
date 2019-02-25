#include "main.h"

#include <string>
#include <fstream>
#include <cstdlib>

#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
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

	// for(query; getline(inputQuery,query); ){
		getline(inputQuery,query);	// gets the first line of queries

		int queryNo = stoi(query.substr(0,2));
		query = query.substr(3);
		cout << endl << query << endl; 
		stringstream query_stream(query);
		while(query_stream >> queryTerm){
			// cout << " original term: " << queryTerm << ".  ";
			transform(queryTerm.begin(), queryTerm.end(), queryTerm.begin(), ::tolower);

			if(index->check_if_stopword(queryTerm)){ cout << queryTerm << " is a stopword." << endl; continue;}

			Porter2Stemmer::trim(queryTerm);
			Porter2Stemmer::stem(queryTerm);

			index->test(queryTerm);

		}
	// }


	return 0;
}