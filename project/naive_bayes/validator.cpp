#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cilk/cilk.h>
#include <cilk/reducer_opadd.h>

#include "naive_bayes.hpp"
#include "validator.hpp"

#define NEWS_20		1
#define REUTERS		2
#define ENRON		3

#define NEWS_20_NAME	"./20news/"	
#define REUTERS_NAME	"./reuters/"	
#define ENRON_NAME	"./enron/"

#define NAIVE_BAYES_CLASSIFIER	1
#define N_GRAM_CLASSIFIER	2

#define NAIVE_BAYES_FILE_NAME	"naive_bayes_classified";
#define N_GRAM_FILE_NAME	"n_gram_classified";

/* #define DEBUG_1 */
/* #define DEBUG_2 */

typedef std::vector<std::string>      categories;

using namespace std;

Validator::Validator(int dataset, int classifier, string* categoryNames, int n)
{
#ifdef DEBUG_2
	cout << "Validator::Validator " << endl;
	cout << dataset << " 	" << classifier << endl;
#endif
	this->dataset = dataset;
	this->classifier = classifier;
	this->categoryNames = categoryNames;
  this->n = n;
	test_set_classified_name = "test_set_classified";
	std::string classified_name;
	
	// what is the dataset?
	switch(dataset){
		case NEWS_20:
			classified_name = NEWS_20_NAME;
			break;
		case REUTERS:
			classified_name = REUTERS_NAME;
			break;
		case ENRON:
			classified_name = ENRON_NAME;
			break;
		default:
			cout << "f-measure:: can't recognize the dataset name" << endl;
			break;
	}
	
	test_set_classified_name = classified_name + "test_set_classified";
#ifdef DEBUG_2
	cout << "test_set_classified_name: " << test_set_classified_name << endl;
#endif
	
	// what is the classifier we are validating?
	switch(classifier){
		case NAIVE_BAYES_CLASSIFIER:
			classified_name += NAIVE_BAYES_FILE_NAME;
			break;
		case N_GRAM_CLASSIFIER:
			classified_name += N_GRAM_FILE_NAME;
			break;
		default:
			cout << "f-measure:: can't recognize the classifier" << endl;
			break;
	}
}

void Validator::f_measure(unordered_map<string, string> &classified)
{	
#ifdef DEBUG_1
	cout << "Validator::f_measure()" << endl;
#endif
	// true values from the test set
	unordered_map<string, string> test_set_classified;
	// values from our classifier are in classified map argument
	std::unordered_map<string,string>::iterator it;
	unordered_map<std::string, std::pair<std::string, std::string> > confussion;
	std::unordered_map<string,std::pair<std::string, std::string> >::iterator itt;
	
	std::map <std::string, int> indeces;
	// confussion matrix
	int** confussionM  = new int*[n];
	double* recall = new double[n];
	double* precission = new double[n];
	double F_Measure = 0;
	double accurancy = 0;
	
	// allocate confussion matrix
	for(int i = 0; i < n; ++i)
	{
		confussionM [i] = new int[n];
	}
	// read results we got
	readCategorizedData(test_set_classified_name, test_set_classified);
	// docId, <true_cat, classified_cat>
	for (it=classified.begin(); it!=classified.end(); ++it){
			// docId is a string
		// it->first is docID followed by pair <truth-cat-str, classified-cat-str>
		confussion[it->first] = std::make_pair(test_set_classified[it->first], it->second);
	}

	// initialize confussion matrix to zeros
	for (int i = 0; i < n; i++){
		for (int j = 0; j < n; j++)
		{
			confussionM[i][j] = 0;
		}
	}
	
	/*
	 Build a map that will give us an index - int for each category name
	 this way we can store the whole matrix as a simple in[][] matrix.
	 */
	for (int index = 0; index < n; index++)
	{
		indeces[categoryNames[index]] = index;
	}
	//TODO EXCEPTIONS!! check for not existing stuff
	for (itt=confussion.begin(); itt!=confussion.end(); ++itt){
		std::pair<std::string, std::string> cats1 = itt->second;
		int i = indeces[cats1.first];	
		int j = indeces[cats1.second];
		confussionM[i][j] +=1;
#ifdef DEBUG_1	
		cout << "true value category: " << cats1.first << endl;
		cout << "naive value category: " << cats1.second << endl;
		cout << confussionM[i][j] << endl;
#endif		
	}
	
#ifdef DEBUG_1	
  printMatrix(confussionM, n);
#endif
	
	// compute recall
	double* temp = new double[n];
	
	double* sum_cij_for_all_j = new double[n];
	
	// second index iterates
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			sum_cij_for_all_j[i] += confussionM[i][j];
		}
	}

	double* sum_cji_for_all_j = new double[n];
	// first index iterates	
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			sum_cji_for_all_j[i] += confussionM[j][i];
		}
	}
	
	for (int i = 0; i < n; i++)
	{
		recall[i] = confussionM[i][i] / sum_cij_for_all_j[i];
	}
	
#ifdef DEBUG_1	
	// print recall per category:
	cout << "recall per category:" << endl;
	for (int i = 0; i < n; i++)
	{
		cout << recall[i] << endl;
	}
	
#endif

	
	// compute precision
	for (int i = 0; i < n; i++)
	{
		precission[i] = confussionM[i][i] / sum_cji_for_all_j[i];
	}
	
#ifdef DEBUG_1
	//print precission per category:
	cout << "precission per category:" << endl;
	for (int i = 0; i < n; i++)
	{
		cout << precission[i] << endl;
	}
#endif
	
	double all_sum = 0;
	
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			all_sum += confussionM[i][j];
		}
	}
	double all_ii = 0;
	
	for (int i = 0; i < n; i++)
	{
		all_ii += confussionM[i][i];
	}	
	
	accurancy = all_ii / all_sum;
	
	cout << "Accuracy: " << accurancy << endl;
	
	// deallocate the matrix
	for (int i = 0; i < n; i++)
	{
		delete [] confussionM[i];
	}	
	delete [] confussionM;
}

void Validator::f_measure_parallel(unordered_map<string, string> &classified)
{	
#ifdef DEBUG_1
	cout << "Validator::f_measure_parallel()" << endl;
#endif
	// true values from the test set
	unordered_map<string, string> test_set_classified;
	// values from our classifier are in classified
	std::unordered_map<string,string>::iterator it;
	// it->first is docID followed by pair <truth-cat-str, classified-cat-str>
	unordered_map<std::string, std::pair<std::string, std::string> > confussion;
	std::unordered_map<string,std::pair<std::string, std::string> >::iterator itt;
	std::map <std::string, int> indeces;
	// confussion matrix
	int** confussionM  = new int*[n];
	double* recall = new double[n];
	double* precission = new double[n];
	double F_Measure = 0;
	double accurancy = 0;
	
	// allocate confussion matrix
	cilk_for(int i = 0; i < n; ++i)
	{
		confussionM [i] = new int[n];
	}
	// results we got are now passed as argument
	// read the true 
#ifdef DEBUG_1
	cout << "test_set_classified_name" << test_set_classified_name << endl;
#endif
	readCategorizedData(test_set_classified_name, test_set_classified);
	// docId, <true_cat, classified_cat>
	for (it=classified.begin(); it!=classified.end(); ++it){
		// docId is a string
		// it->first is docID followed by pair <truth-cat-str, classified-cat-str>
		confussion[it->first] = std::make_pair(test_set_classified[it->first], it->second);
	}
	// initialize confussion matrix to zeros
	cilk_for (int i = 0; i < n; i++){
		for (int j = 0; j < n; j++)
		{
			confussionM[i][j] = 0;
		}
	}
	/*
	 *	 Build a map that will give us an index - int for each category name
	 *	 this way we can store the whole matrix as a simple in[][] matrix.
	 */
	cilk_for (int index = 0; index < n; index++)
	{
		indeces[categoryNames[index]] = index;
	}
	//TODO EXCEPTIONS!! check for not existing stuff
	//cout << "confussion.begin" << endl;
	for (itt=confussion.begin(); itt!=confussion.end(); ++itt){
		std::pair<std::string, std::string> cats1 = itt->second;
		int i = indeces[cats1.first];	
		int j = indeces[cats1.second];
		confussionM[i][j] +=1;
	}
	
#ifdef DEBUG_1
  printMatrix(confussionM, n);
#endif
	
	// compute recall
	double* temp = new double[n];
	
	double* sum_cij_for_all_j = new double[n];
	
	// second index iterates
	cilk_for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			sum_cij_for_all_j[i] += confussionM[i][j];
		}
	}
	
	double* sum_cji_for_all_j = new double[n];
	// first index iterates	
	cilk_for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			sum_cji_for_all_j[i] += confussionM[j][i];
		}
	}
	
	cilk_for (int i = 0; i < n; i++)
	{
		recall[i] = confussionM[i][i] / sum_cij_for_all_j[i];
	}
	
	// compute precision
	cilk_for (int i = 0; i < n; i++)
	{
		precission[i] = confussionM[i][i] / sum_cji_for_all_j[i];
	}
	
	double all_sum = 0;
	
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			all_sum += confussionM[i][j];
		}
	}
	double all_ii = 0;
	
	for (int i = 0; i < n; i++)
	{
		all_ii += confussionM[i][i];
	}	
	
	accurancy = all_ii / all_sum;
	
	cout << "Accuracy: " << accurancy << endl;
	
	// deallocate the matrix
	for (int i = 0; i < n; i++)
	{
		delete [] confussionM[i];
	}	
	delete [] confussionM;
}


void Validator::readCategorizedData(string fileName, unordered_map<string, string> &classified)
{

	ifstream in (fileName);
	if (!in)
	{
		cout << "readCategorizedData::Unable to open the file: " << fileName << endl;
		exit(-1);
	}
	while (in)
	{
		// Read one line at a time
		string line;
		string docId;
		getline(in, line);
		// read the document id
		istringstream iss(line);
		getline(iss, docId, ' ');
		string cat;
		// read the category
		getline(iss, cat, ' ');
		// add to the map
		classified[docId] = cat;
	}
	in.close();
}

void Validator::printMatrix(int** matrix, int n)
{
	//print the confussion_matrix
	cout << "Confusion Matrix: "<< endl;
	for (int i = 0; i < n; i++){
		for (int j = 0; j < n; j++)
		{
			printf("%6d",matrix[i][j]);
		}
		cout << endl;
}
	}