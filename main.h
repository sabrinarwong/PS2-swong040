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

typedef pair<string, int> posting; // (docID, freq of term in doc) ** per term per doc
typedef pair<string, int> termsInDoc; // (docID, total num terms in doc) ** all terms per document 
typedef pair<string, double> tf; // (doc/queryNo, tf);

struct term{
	string t = ""; // term in index
	set<string> ids; // num of docs with term
	vector<posting> postings; // list of term postings

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

struct collection{
	vector<termsInDoc> docCollection; // all docs with num terms
	vector<term> terms; // all terms in collection

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
		term t;
		vector<string> stopList;
		int docCount;
		double term_tf, idf = 0, query_tf;
		vector<tf> querTF, termTF;

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

		void query_index(string fuck, string queryNo){
			// int size = query.docCollection.size();

			// move on to actual query
			stringstream input(fuck);
			string text; int count = 0;
			string inTerms[30];

			while(input >> text){
				count++;

				if(isdigit(text[0])){
					int textInt = stoi(text);
					text = to_string(textInt);
					query.addTerm(text, queryNo);
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
						if(check_if_stopword(inTerms[i])){ continue; }

						Porter2Stemmer::trim(inTerms[i]);
						Porter2Stemmer::stem(inTerms[i]);

						// add to terms in collection
						query.addTerm(inTerms[i], queryNo);
					}
					continue;
				}
				
				if(check_if_stopword(text)){ continue; }

				Porter2Stemmer::trim(text);
				Porter2Stemmer::stem(text);

				query.addTerm(text, queryNo);
			}

			termsInDoc something = make_pair(queryNo, count);
			query.docCollection.push_back(something);
		}

		void initVars(){
			index.docCollection.resize(73);
			docCount = 0;
		}

		void createIndex(string filename){
			// if index isnt already created, initialize.
			// if((index.docCollection.size() <= 1) && (stopList.empty() == true)){
				initVars();
				get_stop_words();

			string inTerms[50];

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
		    		index.docCollection[docCount - 1].first = docNo_line;
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

		    			// parse each word from getline
		    			stringstream check(line);
		    			string word;

						while(check >> word){
							index.docCollection[docCount - 1].second++;

							if(isdigit(word[0])){
								int textInt = stoi(word);
								word = to_string(textInt);
								index.addTerm(word, docNo_line);
								continue;
							}
					
							transform(word.begin(), word.end(), word.begin(), ::tolower); 
							
							int termCount = 0;
							size_t punct = word.find_first_of(",./<>?\\:;\'\"!@#$%^&*(){}[]-_=+");
							while (punct != string::npos){
								inTerms[termCount] = word.substr(0, punct);
								word = word.substr(punct + 1, word.size());
								punct = word.find_first_of(",./<>?\\:;\'\"!@#$%^&*(){}[]-_=+");
								termCount++;
							}
							inTerms[termCount] = word;

							if(termCount > 0){
								for(int i = 0; i < termCount; i++){
									if(check_if_stopword(inTerms[i])){ continue; }

									Porter2Stemmer::trim(inTerms[i]);
									Porter2Stemmer::stem(inTerms[i]);

									index.addTerm(inTerms[i], docNo_line);
								}
								continue;
							}
							
							if(check_if_stopword(word)){ continue; }

							Porter2Stemmer::trim(word);
							Porter2Stemmer::stem(word);

							index.addTerm(word, docNo_line);

		    			}
				    	// get next line	
			    		getline(input,line);
		    		}
		    		continue;
		    	}
		    }
		}

		// fix here // almost
		void get_tfidf(string text){
			cout << "    " << text << endl;
			// calculate tf-idf for query and index 
			int termInd = pos(text, index);
			int queryInd = pos(text, query);
			
			vector<termsInDoc> queryDocs = query.docCollection;
			int qSize = query.terms[queryInd].postings.size(); // num of docs with term

			vector<termsInDoc> indexDocs = index.docCollection;
			int k = 73; // total num of docs
			int iSize = index.terms[termInd].postings.size(); // num of docs with term
			
			// IDF(t) = log_e(Total number of documents / Number of documents with term t in it).
			// idf = 1 + log(k / (double)pSize); // ??? from example
			idf = log(k / (double)iSize);
			if(isinf(idf)){
				idf = 0;
			}			
			// cout << "   " << query.terms[queryInd].t << ": (tf, idf, tf*idf)." << endl;

			// print all queries with posting
			for(int i = 0; i < qSize; i++){
				int n;
				string id = "";
				int freq = query.terms[queryInd].postings[i].second; // freq of term in doc
				for(int j = 0; j < queryDocs.size(); j++){
					if(query.terms[queryInd].postings[i].first == queryDocs[j].first){
						id = queryDocs[j].first;
						n = queryDocs[j].second;	// total num terms in doc
						break;
					}
				}
				// TF(t) = (Number of times term t appears in a document) / (Total number of terms in the document).
				query_tf = freq / (double)n;




				cout << "      " << id 
					 // << " : (" 
					 // << query_tf << ", " 
					 // << idf << ", " 
					 // << query_tf * idf << ")"
					 << endl;

				// for every term in query, print tfidf posting in doc

				// print all docs with term
				for(int i = 0; i < iSize; i++){
					int n;
					string id = "";
					int freq = index.terms[termInd].postings[i].second; // freq of term in doc
					for(int j = 0; j < indexDocs.size(); j++){
						if(index.terms[termInd].postings[i].first == indexDocs[j].first){
							id = indexDocs[j].first;
							n = indexDocs[j].second;	// total num terms in doc
							break;
						}
					}
					// TF(t) = (Number of times term t appears in a document) / (Total number of terms in the document).
					term_tf = freq / (double)n;

					cout << "\t" << id << " : (" 
						 << term_tf << ", " 
						 << idf << ", " 
						 << term_tf * idf << ")" << endl;

					// cosine_similarity();
				}
			}
		}

		void cosine_similarity(){

		}

	public:
		docIndex(){
			createIndex("data/ap89_collection");
		}

		// FIX ME
		// void print(int ind){
		// 	cosine_similarity(ind);
		
		// }

		void query_input(string input){
			// get query no from query list
			string queryNo = (input.substr(0,input.find_first_of(".")));
			input = input.substr(3);

			query.clear();

			// separate query by term
			query_index(input, queryNo);

			// move on to actual query
			stringstream look(input);
			string text;

			// calculate cosine similarity
			while(look >> text){
				// digit query
				if(isdigit(text[0])){
					int textInt = stoi(text);
					text = to_string(textInt);
					get_tfidf(text);
					continue;
				}

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
						if(check_if_stopword(inTerms[i])){ continue; }

						Porter2Stemmer::trim(inTerms[i]);
						Porter2Stemmer::stem(inTerms[i]);

						get_tfidf(inTerms[i]);
					}
					continue;
				}

				Porter2Stemmer::trim(text);
				Porter2Stemmer::stem(text);

				if(check_if_stopword(text)){
					// cout << text << " is a stopword. " << endl;
					continue;
				}
				get_tfidf(text);
			}

		}

		bool check_if_stopword(string input){
			for(unsigned i = 0; i < stopList.size(); i++){
				if(strcmp(input.c_str(), stopList[i].c_str()) == 0){
					return true;
				}
			}
			return false;
		}

		int pos(string input, collection collection){
			int ind = -1;
			for(int i = 0; i < collection.terms.size(); i++){
				if(strcmp(input.c_str(), collection.terms[i].t.c_str()) == 0){
					ind = i;
					break;
				}
			}

			if(ind == -1){	// if term is in index
				cout << input << " not found in index." << endl;
			}
			return ind;
		}

		//testing purposes
		void print_index(){
			// output to docIndex for 2nd index creation
			ofstream output; output.open("index.txt");
			output << "Document Index contains:\n";

			for(int i = 0; i < index.docCollection.size(); i++){
				termsInDoc k = index.docCollection[i];
				output << "Doc " << k.first << ": " << k.second << " terms\n";
			}

			output << "\nPosting List contains:\n";
			for(int i = 0; i < index.terms.size(); i++){
				term k = index.terms[i];
				output << k.t << "(" << k.ids.size() << ")" << endl << "\t";
				for(int j = 0; j < k.postings.size(); j++){
					output << "(" << k.postings[j].first << ": " << k.postings[j].second << ") ";
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
				output << "Doc " << k.first << ": " << k.second << " terms\n";
			}

			output << "\nPosting List contains:\n";
			for(int i = 0; i < query.terms.size(); i++){
				term k = query.terms[i];
				output << k.t << "(" << k.ids.size() << ")" << endl << "\t";
				for(int j = 0; j < k.postings.size(); j++){
					output << "(" << k.postings[j].first << ": " << k.postings[j].second << ") ";
				}
				output << endl;
			}

			cout << "   Query Index file created." << endl;

			output.close();
		}
};

#endif