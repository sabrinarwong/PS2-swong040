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
		int i, ind = -1;
		for(i = 0; i < terms.size(); i++){
			// if word is in collection, increment posting 
			if(strcmp(word.c_str(), terms[i].t.c_str()) == 0){
				ind = i;
				terms[ind].addPosting(word, docID);
				return;
			}
		}
		/* NTS: this ^^ function is a tad different */

		// if word isnt in collection, make new posting and add to collection
		if(ind == -1){
			term x;
			x.addPosting(word, docID);
			terms.push_back(x);
			return;
		}
	}
};

class docIndex{
	private:
		collection index;
		term t;
		vector<string> stopList;
		int docCount;

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
			cout << "   Stop Words vector created." << endl;

			// for(int i = 0; i < stopList.size(); i++){ cout << stopList[i] << endl; }
		}

		void tfidf(int termInd, vector<termsInDoc> docs){
			int k = 73; // total num of docs
			int pSize = index.terms[termInd].postings.size(); // num of docs with term
			
			// IDF(t) = log_e(Total number of documents / Number of documents with term t in it).
			// double idf = 1 + log(k / (double)pSize); // ??? from example
			double idf = log(k / (double)pSize);

			cout << "   " << index.terms[termInd].t << ": (tf, idf, tf*idf)." << endl;

			for(int i = 0; i < pSize; i++){
				int n;
				string id = "";
				int freq = index.terms[termInd].postings[i].second; // freq of term in doc
				for(int j = 0; j < docs.size(); j++){
					if(index.terms[termInd].postings[i].first == docs[j].first){
						id = docs[j].first;
						n = docs[j].second;	// total num terms in doc
						break;
					}
				}
				// TF(t) = (Number of times term t appears in a document) / (Total number of terms in the document).
				double tf = freq / (double)n;

				cout << "\t" << id << " : (" 
					 << tf << ", " 
					 << idf << ", " 
					 << tf * idf << ")" << endl;
			}
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
					
							transform(word.begin(), word.end(), word.begin(), ::tolower); 
							
							int stopFlag = false, termCount = 0;
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
									for(int j = 0; j < stopList.size(); j++){
										if(strcmp(inTerms[i].c_str(), (stopList[j]).c_str()) == 0){
											stopFlag = true;
											break;
										}
									}
									if(stopFlag){ continue; }

									Porter2Stemmer::trim(inTerms[i]);
									Porter2Stemmer::stem(inTerms[i]);

									index.addTerm(inTerms[i], docNo_line);
								}
								continue;
							}
							for(int i = 0; i < stopList.size(); i++){
								if(strcmp(word.c_str(), (stopList[i]).c_str()) == 0){
									stopFlag = true;
									break;
								}
							}
							if(stopFlag){ continue; }

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

	public:
		docIndex(){
			createIndex("data/ap89_collection");
		}

		void print(int ind){
			tfidf(ind, index.docCollection);
		}

		bool check_if_stopword(string input){
			for(unsigned i = 0; i < stopList.size(); i++){
				if(strcmp(input.c_str(), stopList[i].c_str()) == 0){
					return true;
				}
			}
			return false;

		}

		void test(string input){
			int i;
			int ind = -1;
			for(i = 0; i < index.terms.size(); i++){
				if(strcmp(input.c_str(), index.terms[i].t.c_str()) == 0){
					ind = i;
					break;
				}
			}

			if(ind == -1){	// if term is in index
				cout << input << " not found in index." << endl;
				return;
			}
			print(ind);
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

};

#endif