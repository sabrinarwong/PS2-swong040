/* 
	FROM PS1 MODIFIED TO INCLUDE STEMMING AND RENAMED SOME FUNCTIONS FOR BETTER UNDERSTANDING

	header file creating the inverted index with stemming
	takes filename as input
	returns index, adding terms if one already exists

*/

#ifndef MAIN_H
#define MAIN_H

#include "porter2_stemmer/porter2_stemmer.h"

#include <algorithm>
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

struct tfidf{
	string text = ""; 			// term
	string docId = ""; 			// doc_id / query_no
	double tf_idf = 0;		// tfidf value
};

struct cosSim{
	string docId = "";
	double cs_val = 0;
};

bool compare_cosSims(cosSim val1, cosSim val2){
	return (val1.cs_val > val2.cs_val);
}

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

	// return the index of doc in posting
	int post_pos(string input){
		int ind = -1;
		for(int i = 0; i < postings.size(); i++){
			if(strcmp(input.c_str(), postings[i].first.c_str()) == 0){
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

	// return the index of term in the collection
	int term_pos(string input){
		int ind = -1;
		for(int i = 0; i < terms.size(); i++){
			if(strcmp(input.c_str(), terms[i].t.c_str()) == 0){
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

	// return the index of doc in the collection
	int doc_pos(string input){
		int ind = -1;
		for(int i = 0; i < docCollection.size(); i++){
			if(strcmp(input.c_str(), docCollection[i].first.c_str()) == 0){
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
};

class docIndex{
private:
	collection index, query;
	int docCount;
	vector<string> stopList;
	vector<tfidf> t_tfidf, q_tfidf;
	vector<cosSim> cosine_similarity;

	// index of tfidf value in vector
	int tfidf_pos(string text, vector<tfidf> v_tfidf){
		int ind = -1;
		for(int i = 0; i < v_tfidf.size(); i++){
			if(strcmp(text.c_str(), v_tfidf[i].text.c_str()) == 0){
				ind = i;
				break;
			}
		}
	}

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

	void add_to_coll(string & fuck, string queryNo, collection & coll, bool checkOn){
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
					if(checkOn && check_if_stopword(inTerms[i])){ continue; }

					// add to terms in collection
					coll.addTerm(inTerms[i], queryNo);
				}
				continue;
			}
			
			Porter2Stemmer::trim(text);
			Porter2Stemmer::stem(text);
			if(checkOn && check_if_stopword(text)){ continue; }

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

	    			add_to_coll(line, docNo_line, index, true);
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

		// separate query by term
		add_to_coll(word, queryNo, query, false);
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
	*/

	// TF(t) = (Number of times term t appears in a document) / (Total number of terms in the document).
	double get_tf(string text, string docNo, collection coll){
		int termPos = coll.term_pos(text),
			docPos = coll.doc_pos(docNo),
			postPos = coll.terms[docPos].post_pos(docNo);

		int freq = coll.terms[termPos].postings[postPos].second,
			total = coll.docCollection[docPos].second;
		
		return (freq / (double)total);
	}

	// IDF(t) = log_e(Total number of documents / Number of documents with term t in it).
	double get_idf(string text){
		int termPos = index.term_pos(text),
			size = index.terms[termPos].postings.size(); // num of docs with term

		// idf = 1 + log(k / (double)pSize); // ??? from example
		double idf = 1 + log(73 / (double)size);
		if(isinf(idf) || isnan(idf)){
			idf = 0;
		}
		return idf;
	}

	// gets cosine similarity of between query and docId
	double get_cosine_similarity(string docId){
		double q_length = 0, t_length = 0, dot_sum = 0;
		for(int i = 0; i < q_tfidf.size(); i++){
			q_length += pow(q_tfidf[i].tf_idf, 2);
		}
		q_length = sqrt(q_length);

		for(int i = 0; i < t_tfidf.size(); i++){
			if(strcmp(t_tfidf[i].docId.c_str(), docId.c_str()) == 0){
				t_length += pow(t_tfidf[i].tf_idf, 2);

				int qPos = tfidf_pos(t_tfidf[i].text, q_tfidf);
				dot_sum += t_tfidf[i].tf_idf * q_tfidf[qPos].tf_idf;
			}
		}
		t_length = sqrt(t_length);

		return (dot_sum / (q_length * t_length));
	}

	// fill vector with tfidf of query terms
	void get_query_tfidf(){
		q_tfidf.clear();	// reset every query

		double tf, idf;
		tfidf addToQuery;
		for(int i = 0; i < query.terms.size(); i++){
			addToQuery.text = query.terms[i].t;
			addToQuery.docId = query.docCollection[0].first;

			tf = get_tf(addToQuery.text, addToQuery.docId, query);
			idf = get_idf(addToQuery.text);
			addToQuery.tf_idf = tf * idf;

			q_tfidf.push_back(addToQuery);
		}
	}

	// get term tfidf matching the query tfidf vector
	void get_term_tfidf(){
		t_tfidf.clear();	// reset every query

		double tf, idf;
		tfidf addToTerm;
		for(int i = 0; i < q_tfidf.size(); i++){
			addToTerm.text = q_tfidf[i].text;

			int termPos = index.term_pos(addToTerm.text);
			if(termPos == -1){ continue; }

			for(int k = 0; k < index.terms[termPos].postings.size(); k++){
				addToTerm.docId = index.terms[termPos].postings[k].first;
				
				tf = get_tf(addToTerm.text, addToTerm.docId, index);
				idf = get_idf(addToTerm.text);
				addToTerm.tf_idf = tf * idf;

				t_tfidf.push_back(addToTerm);
			}
		}
	}

	// fill cos_sim vector and order them
	void create_cos_sim(){
		cosine_similarity.clear(); 	// reset for each query

		int docCount = 1;
		while(docCount < 74){
			ostringstream docNo_line;
			docNo_line << std::setw(4) << std::setfill('0') << docCount;
			string docNo = "AP890101-" + docNo_line.str();

			cosSim addToCosSim;
			addToCosSim.docId = docNo;
			addToCosSim.cs_val = get_cosine_similarity(docNo);
			if(addToCosSim.cs_val != 0){
				cosine_similarity.push_back(addToCosSim);
			}
			docCount++;
		}
		stable_sort(cosine_similarity.begin(), cosine_similarity.end(), compare_cosSims);
	}

	void generate_results_file(string queryNo){
		ofstream output;
		output.open("results_file.txt", fstream::app);
		if(!output.is_open()){
			cout << "Error opening results_file.txt" << endl;
			return;
		}

		int rank = 1;
		for(int i = 0; i < cosine_similarity.size(); i++){
			if(rank < 101){
				output << queryNo << " Q0 " << cosine_similarity[i].docId << " " 
			 		   << rank << " " << cosine_similarity[i].cs_val << " Exp" 
			 		   << endl;
			}
			rank++;
		}
		output.close();
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
		get_query_tfidf();
		print_tfidf('q', q_tfidf);
		get_term_tfidf();
		print_tfidf('t', t_tfidf);

		create_cos_sim();
		generate_results_file(queryNo);
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

	//testing purposes
	void print_tfidf(char a, vector<tfidf> v_tfidf){
		// output to docIndex for 2nd index creation
		string filename = string(1, a) + "_tfidf.txt";

		ofstream output; output.open(filename, fstream::app);
		output << filename << " vector contains:\n";

		for(int i = 0; i < v_tfidf.size(); i++){
			tfidf k = v_tfidf[i];
			output << k.text << " : " 
				   << k.docId << " " 
				   << k.tf_idf << endl;
		}
		output << endl;

		// cout << "   tfidf file created." << endl;

		output.close();
	}
};

#endif