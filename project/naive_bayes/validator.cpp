#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

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



typedef std::vector<std::string>      categories;

using namespace std;

Validator::Validator(int dataset, int classifier, string* categoryNames, int categoryNames_size)
{
	cout << "Validator::Validator " << endl;
	cout << dataset << " 	" << classifier << endl;
	this->dataset = dataset;
	this->classifier = classifier;
	//validate(dataset, classifier);
	f_measure(categoryNames, categoryNames_size);
}


/* method goes to the dataset directory and compares
 * the values from the given classifier and real values */
void validate(int dataset, int classifier)
{
  double F_measure = 0;
  double precission = 0;
  double recall = 0;
  int correct = 0;
  int incorrect = 0;
  string test_set_classified_name = "test_set_classified";
  std::string classified_name;
  // true values from the test set
  unordered_map<string, categories > test_set_classified;
  // values from our classifier
  unordered_map<string, categories > classified;
  std::unordered_map<string,categories>::iterator it;

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
      cout << "validate:: can't recognize the dataset name" << endl;
      break;
  }
  
  test_set_classified_name = classified_name + "test_set_classified";
  
  // what is the classifier we are validating?
    switch(classifier){
    case NAIVE_BAYES_CLASSIFIER:
      classified_name += NAIVE_BAYES_FILE_NAME;
      break;
    case N_GRAM_CLASSIFIER:
      classified_name += N_GRAM_FILE_NAME;
      break;
    default:
      cout << "validate:: can't recognize the classifier" << endl;
      break;
  }
  
  // TODO move those reads to a separate method.
  
  // read results we got
  ifstream inputFile (classified_name);
  if (!inputFile)
  {
    cout << "validate::Unable to open the file: " << classified_name << endl;
    exit(-1);
  }
  // While there's still stuff left to read...
  while (inputFile)
  {
    // Read one line at a time
    string line;
    string docId;
    string catName;
    getline(inputFile, line);
    // read the document id
    istringstream iss(line);
    getline(iss, docId, ' ');
    categories cats;
    string cat;
    // read all the topics that are associated to this docId
    while (getline(iss, cat, ' '))
    {
      // and add them to the list of topics for that docId
      classified[docId].push_back(cat);
    }
  }
 inputFile.close();
  // read the true values
  ifstream inputFilea (test_set_classified_name);
  if (!inputFilea)
  {
    cout << "validate::Unable to open the file: " << test_set_classified_name << endl;
    exit(-1);
  }
  while (inputFilea)
  {
    // Read one line at a time
	string line;
    string docId;
    string catName;
    getline(inputFilea, line);
    // read the document id
    istringstream iss(line);
    getline(iss, docId, ' ');
    categories cats;
    string cat;
    // read all the topics that are associated to this docId
    while (getline(iss, cat, ' '))
    {
      // and add them to the list of topics for that docId
      test_set_classified[docId].push_back(cat);
    }
  }
   inputFilea.close();
  // compares
	// for now check accurancy num-of-correct/total-num
   // itterate through all the docId's in the file
	for (it=classified.begin(); it!=classified.end(); ++it){
		// for each document, get ID
		// check if there is one in the test set (it should be)
		// TODO check exceptions
		string doc_Id = it->first;
		categories cats = it->second;
		string found_cat = cats.front();
		cout << "front " + found_cat << endl; 
		
		// now get the coresponding true value from the test set:
		categories real_cats = test_set_classified[doc_Id];
		// do they have our category?
		if(std::find(real_cats.begin(), real_cats.end(), found_cat)!=real_cats.end())
		{
			cout << "found " + found_cat << endl; 
			correct++;
		}else
		{
			cout << "didn't find " + found_cat << endl;
			incorrect++;
		}
		
		//cout << "sec " << it->second << endl;
	}
  // got through classified and 
  // for each docId 
  // check if its category belongs to the list of categories
  // from the test_set_classified list for the same docId.
  
  // calculate
  
}


void Validator::f_measure(string* categoryNames, int categoryNames_size)
{	
	cout << "Validator::f_measure()" << endl;
	double F_measure = 0;
	double precission = 0;
	double recall = 0;
	int correct = 0;
	int incorrect = 0;
	string test_set_classified_name = "test_set_classified";
	std::string classified_name;
	// true values from the test set
	unordered_map<string, string> test_set_classified;
	// values from our classifier
	unordered_map<string, string> classified;
	std::unordered_map<string,std::pair<std::string, std::string> >::iterator itt;
	std::unordered_map<string,string>::iterator it;
	std::map<std::pair<std::string, std::string> , int> conf_mat_it;
	
	std::map<string,string>::iterator itm;
	
	unordered_map<std::string, std::pair<std::string, std::string> > confussion;
	std::map<std::pair<std::string, std::string> , int> confussion_matrix;
	
	int dataset = this->dataset;
	int classifier = this->classifier;
	//double** confussionM;
	
	int** confussionM  = new int*[categoryNames_size];
	for(int i = 0; i < categoryNames_size; ++i)
	{
		confussionM [i] = new int[categoryNames_size];
	}
	
	// TODO move all this to constructor
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
	cout << "test_set_classified_name: " << test_set_classified_name << endl;
	
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
	
	// TODO move those reads to a separate method.
	
	// read results we got
	ifstream inputFile (classified_name);
	if (!inputFile)
	{
		cout << "f-measure::Unable to open the file: " << classified_name << endl;
		exit(-1);
	}
	// While there's still stuff left to read...
	while (inputFile)
	{
		// Read one line at a time
		string line;
		string docId;
		string catName;
		getline(inputFile, line);
		// read the document id
		istringstream iss(line);
		getline(iss, docId, ' ');
		categories cats;
		string cat;
		// read all the topics that are associated to this docId
		getline(iss, cat, ' ');
		// and add them to the list of topics for that docId
		classified[docId] = cat;
		
	}
	inputFile.close();
	// read the true values
	ifstream inputFilea (test_set_classified_name);
	if (!inputFilea)
	{
		cout << "f-measure::Unable to open the file: " << test_set_classified_name << endl;
		exit(-1);
	}
	while (inputFilea)
	{
		// Read one line at a time
		string line;
		string docId;
		string catName;
		getline(inputFilea, line);
		// read the document id
		istringstream iss(line);
		getline(iss, docId, ' ');
		categories cats;
		string cat;
		// read all the topics that are associated to this docId
		getline(iss, cat, ' ');
		// and add them to the list of topics for that docId
		test_set_classified[docId] = cat;
	}
	inputFilea.close();
	// docId, <true_cat, classified_cat>
	// TODO test for all kinds of exceptions!!!!
	for (it=classified.begin(); it!=classified.end(); ++it){
		// it's a string
		// it->first is docID followed by pair <truth-str, classified-str>
		confussion[it->first] = std::make_pair(test_set_classified[it->first], it->second);
		//cout << it->second << endl;
		
	}
	
	for (int i = 0; i < categoryNames_size; i++){
		for (int j = 0; j < categoryNames_size; j++)
		{
			confussionM[i][j] = 0;
		}
	}
	
	std::map <std::string, int> indeces;
	
	for (int index = 0; index < categoryNames_size; index++)
	{
		indeces[categoryNames[index]] = index;
	}	
	
	for (int index = 0; index < categoryNames_size; index++)
	{
		indeces[categoryNames[index]] = index;
	}
	
	//EXCEPTIONS!!
	for (itt=confussion.begin(); itt!=confussion.end(); ++itt){
		std::pair<std::string, std::string> cats1 = itt->second;
		int i = indeces[cats1.first];
		int j = indeces[cats1.second];
		//cout << "put " << i << endl;
		confussionM[i][j] +=1;
	}
	
	cout << "confussion matrix: "<< endl;
	
	for (int i = 0; i < categoryNames_size; i++){
		//cout << "row" << endl;
		for (int j = 0; j < categoryNames_size; j++)
		{
			cout << confussionM[i][j];
			cout << " ";
		}
		cout << endl;
	}
	
	
	for (int i = 0; i < categoryNames_size; i++)
	{
		delete [] confussionM[i];
	}
	
	delete [] confussionM;
	
	
}
	