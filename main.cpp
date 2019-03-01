#include "main.h"
#include <unistd.h>

// change to take input from query_list.txt 
int main(){

	// create index here
	// cout << "Creating index." << endl;
	docIndex *index = new docIndex();

	// index->print_index(); // testing purposes
	cout << "   Index created." << endl;

	ifstream inputQuery;
	string query;

	inputQuery.open("data/query_list.txt");
	if(!inputQuery.is_open()){
		cout << "Error opening query file." << endl;
		return -1;
	}

	cout << "   Making results file." << " Please Wait";

	for(query; getline(inputQuery, query); ) {
		// getline(inputQuery,query);	// gets the first line of queries
		index->queryIndex(query);

        sleep(1);
        std::cout << "." << std::flush;
        sleep(1);
        std::cout << "." << std::flush;
        sleep(1);
        std::cout << "." << std::flush;
        sleep(1);
        std::cout << "\b\b\b   \b\b\b" << std::flush;
		// index->print_query(); // testing purposes
	}
	cout << endl << "   Results file created" << endl;

	return 0;
}