#include "main.h"

// change to take input from query_list.txt 
int main(){

	// create index here
	// cout << "Creating index." << endl;
	docIndex *index = new docIndex();

	index->print_index(); // testing purposes
	cout << "   Index created." << endl << endl;

	ifstream inputQuery;
	string query;

	inputQuery.open("data/query_list.txt");
	if(!inputQuery.is_open()){
		cout << "Error opening query file." << endl;
		return -1;
	}

	for(query; getline(inputQuery, query); ){
		// getline(inputQuery,query);	// gets the first line of queries
		index->queryIndex(query);
	}
	index->print_query(); // testing purposes

	return 0;
}