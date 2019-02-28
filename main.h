/* 
	FROM PS1 MODIFIED TO INCLUDE STEMMING AND RENAMED SOME FUNCTIONS FOR BETTER UNDERSTANDING

	header file creating the inverted index with stemming
	takes filename as input
	returns index, adding terms if one already exists

*/

#ifndef MAIN_H
#define MAIN_H

#include "porter2_stemmer/porter2_stemmer.h"

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


using namespace std;

typedef pair<string, int> posting; 		// (docID, freq of term in doc) ** per term per doc
typedef pair<string, int> termsInDoc; 	// (docID, total num terms in doc) ** all terms per document 

struct term{
	string t = ""; 				// term in index
	set<string> ids; 			// num of docs with term
	vector<posting> postings; 	// list of term postings

	// add posting to term
	void addPosting(string word, string docID){
		t = word;
		for(set<string>::iterator i = ids.begin(); i != ids.end(); i++){
			if(strcmp((*i).c_str(), docID.c_str()) == 0){
				for(int j = 0; j < postings.size(); j++){
					// if term already exists in doc, increment
					if(postings[j].first == docID){
						postings[j].second++;
						return;
					}
				}
			}
		}
		// else create new posting
		ids.insert(docID);
		posting p = make_pair(docID, 1);
		postings.push_back(p);
	}
};

struct tfidf{
	string text = ""; 			// term
	string queryNo = ""; 		// query_no
	string docId = ""; 			// doc_id
	double term_tf_idf = 0;		// tfidf value
	double query_tf_idf = 0;	// tfidf value
};

struct collection{
	string name;
	vector<termsInDoc> docCollection; 	// all docs with num terms
	vector<term> terms; 				// all terms in collection

	//add term into the collection
	void addTerm(string word, string docID){
		int ind = -1;
		for(int i = 0; i < terms.size(); i++){
			// if word is in collection, increment posting 
			if(strcmp(word.c_str(), terms[i].t.c_str()) == 0){
				ind = i;
				terms[ind].addPosting(word, docID);
				return;
			}
		}

		// if word isnt in collection, make new posting and add to collection
		if(ind == -1){
			term x;
			x.addPosting(word, docID);
			terms.push_back(x);
			return;
		}
	}

	// clear similar to clear() for vector, clears collection
	void clear(){
		docCollection.clear();
		terms.clear();
	}
};

class docIndex{
private:
	collection index, query;
	int docCount;
	vector<string> stopList;
	vector<tfidf> tfidf_vals;

	// stop words from .txt to vector
	void get_stop_words(){
		// used to take out stop words in data files
		ifstream stopWords; 
		stopWords.open("stoplist.txt");
		if(stopWords.is_open()){
			while(!stopWords.eof()){
				string stop;
				stopWords >> stop;
				stopList.push_back(stop);
			}
		}
		stopWords.close();
		// cout << "   Stop Words vector created." << endl;
	}

	// checks if input is a stop word
	bool check_if_stopword(string input){
		for(unsigned i = 0; i < stopList.size(); i++){
			if(strcmp(input.c_str(), stopList[i].c_str()) == 0){
				return true;
			}
		}
		return false;
	}

	// return the index of term in the collection
	int pos(string input, collection collection){
		int ind = -1;
		for(int i = 0; i < collection.terms.size(); i++){
			if(strcmp(input.c_str(), collection.terms[i].t.c_str()) == 0){
				ind = i;
				break;
			}
		}
		// if term is in index
		// if(ind == -1){
			// cout << input << " not found in index." << endl;
		// }
		return ind;
	}

	void add_to_coll(string & fuck, string queryNo, collection & coll){
		int size = coll.docCollection.size();

		// move on to actual query
		stringstream input(fuck);
		string text; 
		string inTerms[30];

		bool docInColl = false;
		for(int i = 0; i < coll.docCollection.size(); i++){
			if(strcmp(coll.docCollection[i].first.c_str(), queryNo.c_str()) == 0){
				docInColl = true;
				continue;
			}
		}
		if(coll.docCollection.empty() || docInColl == false){
			termsInDoc something = make_pair(queryNo, 0);
			coll.docCollection.push_back(something);
		}

		while(input >> text){
			for(int i = 0; i < coll.docCollection.size(); i++){
				if(strcmp(coll.docCollection[i].first.c_str(), queryNo.c_str()) == 0){
					coll.docCollection[i].second += 1;
					break;
				}
			}

			if(isdigit(text[0])){
				int textInt = stoi(text);
				text = to_string(textInt);
				coll.addTerm(text, queryNo);
				continue;
			}
				
			transform(text.begin(), text.end(), text.begin(), ::tolower); 
			
			int termCount = 0;
			size_t punct = text.find_first_of(",./<>?\\:;\'\"!@#$%^&*(){}[]-_=+");
			while (punct != string::npos){
				inTerms[termCount] = text.substr(0, punct);
				text = text.substr(punct + 1, text.size());
				punct = text.find_first_of(",./<>?\\:;\'\"!@#$%^&*(){}[]-_=+");
				termCount++;
			}
			inTerms[termCount] = text;

			if(termCount > 0){
				for(int i = 0; i < termCount; i++){
					Porter2Stemmer::trim(inTerms[i]);
					Porter2Stemmer::stem(inTerms[i]);
					if(check_if_stopword(inTerms[i])){ continue; }

					// add to terms in collection
					coll.addTerm(inTerms[i], queryNo);
				}
				continue;
			}
			
			Porter2Stemmer::trim(text);
			Porter2Stemmer::stem(text);
			if(check_if_stopword(text)){ continue; }

			coll.addTerm(text, queryNo);
		}
	}

	void createIndex(string filename){
		index.name = "Index";
		get_stop_words();

		string inTerms[50];
		int docCount = 0;

		// open file input to index
		ifstream input;
		input.open(filename);
		if(!input.is_open()){
			cout << "Error opening file." << endl; 
			return;
		}

		/* ----- parsing each line ----- */

		string docNo_line;
		// get each line from file
	    for (string line; getline(input, line);){

	    	/* 
				1. each document begins with <DOC> and ends with </DOC>
				2. several lines contain metdata data. read <DOCNO> as document ID
				3. document contents are between <TEXT> and </TEXT>
				4. other file contents can be ignored
	    	*/
	    	if(strcmp(line.c_str(), "<DOC>") == 0){
	    		docCount++;
	    		continue;
	    	}
	    	// get docNo string
	    	else if((strlen(line.c_str()) == 30) && (strcmp(line.substr(0,8).c_str(), "<DOCNO> ") == 0)){
	    		docNo_line = line.substr(8,13);
	    		continue;
	    	}
    		// get content from <TEXT> section
	    	else if(strcmp(line.c_str(), "<TEXT>") == 0){
	    		// get the next line
	    		getline(input, line);
	    		while((strcmp(line.c_str(), "</TEXT>") != 0)){
	    			// check end of document content
		    		if(strcmp(line.c_str(), "</TEXT>") == 0){
		    			break;
	    			}

	    			add_to_coll(line, docNo_line, index);
			    	// get next line	
		    		getline(input,line);
	    		}
	    		continue;
	    	}
	    }
	}

	void createQueryIndex(string word, string queryNo){
		// new vector of terms for each query
		query.clear();
		tfidf_vals.clear();

		// separate query by term
		add_to_coll(word, queryNo, query);
	}

	void add_to_tfidf(string t, string q, string d, double i){

		tfidf value;
		int freq = 0, total = 0, ind = pos(t, query);
		total = query.docCollection[0].second;
		freq = query.terms[ind].postings[0].second;

		value.text = t;
		value.queryNo = q;
		value.docId = d;
		value.term_tf_idf = i;
		value.query_tf_idf = freq / (double)total;

		tfidf_vals.push_back(value);
	}

	// TF(t) = (Number of times term t appears in a document) / (Total number of terms in the document).
	void tf(string text, string queryNo, collection coll){
		
		// // Number of times term t appears in a document
		// int freq_term = coll.terms[termInd].postings[postInd].second;
		// // Total number of terms in the document
		// int tot_num_terms = coll.docCollection[docInd].second;
		
		int termInd = -1, postInd = -1, docInd = -1, freq = 0, total = 0;
		string someID = "";

		// get index of term
		for(int i = 0; i < coll.terms.size(); i++){
			term k = coll.terms[i];
			if(strcmp(k.t.c_str(), text.c_str()) == 0){
				termInd = i;
				
				break;
			}
		}

		// get docid and freq at docid
		for(int j = 0; j < coll.terms[termInd].postings.size(); j++){
			posting k = coll.terms[termInd].postings[j];
			someID = k.first;
			freq = k.second;
			
			// get total num terms from docid
			for(int j = 0; j < coll.docCollection.size(); j++){
				termsInDoc k = coll.docCollection[j];
				if(strcmp(k.first.c_str(), someID.c_str()) == 0){
					docInd = j;
					total = k.second;

					double tf = freq / (double)total;
					double tf_idf = tf * idf(text);

					add_to_tfidf(text, queryNo, someID, tf_idf );

				}
			}		
		}
	}

	// IDF(t) = log_e(Total number of documents / Number of documents with term t in it).
	double idf(string text){
		int ind = pos(text, index),
			size = index.terms[ind].postings.size(); // num of docs with term

		// idf = 1 + log(k / (double)pSize); // ??? from example
		double idf = 1 + log(73 / (double)size);
		if(isinf(idf) || isnan(idf)){
			idf = 0;
		}
		return idf;
	}

	/*
		to calculate cosine similarity
		1. normalize tf for each doc
			TF_n = # freq term / total # term
		2. get idf for each doc
			idf = log_e (total # doc / # docs with term)
		3. for each doc per term
			tf_idf = tf * idf
		4. cosSim (Q, D1) = dot product of (q, d1) / |q||d1|
			a. get tfidf of query each term
			b. dot product(q, d1)
				= sum( tfidf_q * tfidf_d1)
			c. |q| =  sqrt(sum(tfid_q ^ 2))
			d. |d1| =  sqrt(sum(tfid_d1 ^ 2))
		}
	*/
	void cosineSimilarity(){
		string docNo_line;



		
	}

public:
	docIndex(){
		createIndex("data/ap89_collection");
	}	

	// string input is each line of query
	void queryIndex(string input){
		// get query no from query list
		string queryNo = (input.substr(0,input.find_first_of(".")));
		input = input.substr(3);

		// create query index
		createQueryIndex(input, queryNo);

		get_tf(input, queryNo, index);
		cosineSimilarity(input);
		// print_ifidf(); // good
	}

	void get_tf(string input, string queryNo, collection coll){
		// need to get each word
		stringstream look(input);
		string text;

		while(look >> text){
			transform(text.begin(), text.end(), text.begin(), ::tolower); 
			size_t punct = text.find_first_of(",./<>?\\:;\'\"!@#$%^&*(){}[]-_=+");
			int termCount = 0;
			string inTerms[30];
			while (punct != string::npos){
				inTerms[termCount] = text.substr(0, punct);
				text = text.substr(punct + 1, text.size());
				punct = text.find_first_of(",./<>?\\:;\'\"!@#$%^&*(){}[]-_=+");
				termCount++;
			}
			inTerms[termCount] = text;
			if(termCount > 0){
				for(int i = 0; i < termCount; i++){
					Porter2Stemmer::trim(inTerms[i]);
					Porter2Stemmer::stem(inTerms[i]);
					if(check_if_stopword(inTerms[i])){ continue; }

					tf(inTerms[i], queryNo, coll);
				}
				continue;
			}
			Porter2Stemmer::trim(text);
			Porter2Stemmer::stem(text);
			if(check_if_stopword(text)){ continue; }

			tf(text, queryNo, coll);
		}
	}

	//testing purposes
	void print_ifidf(){
		// output to docIndex for 2nd index creation
		ofstream output; output.open("tf.txt");
		output << "tfidf vector contains:\n";

		for(int i = 0; i < tfidf_vals.size(); i++){
			tfidf k = tfidf_vals[i];
			output << k.queryNo << " : " 
				   << k.text << ": "
				   << k.docId << " " 
				   << k.term_tf_idf << " "
				   << k. query_tf_idf << endl;
		}

		cout << "   tfidf file created." << endl;

		output.close();
	}

	//testing purposes
	void print_index(){
		// output to docIndex for 2nd index creation
		ofstream output; output.open("index.txt");
		output << "Document Index contains:\n";

		for(int i = 0; i < index.docCollection.size(); i++){
			termsInDoc k = index.docCollection[i];
			output << "Doc " << k.first 	// docID
				   << ": " << k.second		// total num docs
				   << " terms\n";
		}

		output << "\nPosting List contains:\n";
		for(int i = 0; i < index.terms.size(); i++){
			term k = index.terms[i];
			output << k.t << "(" << k.ids.size() << ")" << endl << "\t";
			for(int j = 0; j < k.postings.size(); j++){
				output << "(" << k.postings[j].first 	// docID
					   << ": " << k.postings[j].second	// freq
					   << ") ";
			}
			output << endl;
		}

		cout << "   Document Index file created." << endl;

		output.close();
	}

	//testing purposes
	void print_query(){
		// output to docIndex for 2nd index creation
		ofstream output; output.open("query.txt");
		output << "Query Index contains:\n";

		for(int i = 0; i < query.docCollection.size(); i++){
			termsInDoc k = query.docCollection[i];
			output << "Doc " << k.first 	// docID
				   << ": " << k.second		// total num docs
				   << " terms\n";
		}

		output << "\nPosting List contains:\n";
		for(int i = 0; i < query.terms.size(); i++){
			term k = query.terms[i];
			output << k.t << "(" << k.ids.size() << ")" << endl << "\t";
			for(int j = 0; j < k.postings.size(); j++){
				output << "(" << k.postings[j].first 	// docID
					   << ": " << k.postings[j].second	// freq
					   << ") ";
			}
			output << endl;
		}

		cout << "   Query Index file created." << endl;

		output.close();
	}
};

#endif