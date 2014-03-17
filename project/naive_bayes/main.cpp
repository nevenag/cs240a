#include <cilk/cilk.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "naive_bayes.hpp"
#include "validator.hpp"
#include "example_util_gettime.h"

#define NEWS_20		1
#define REUTERS		2
#define ENRON		3

#define NEWS_20_NAME	"./20news/"	
#define REUTERS_NAME	"./reuters/"	
#define ENRON_NAME	"./enron/"

#define SEQUENTIAL_EXECUTION 1
#define PARALLEL_EXECUTION 2

#define NAIVE_BAYES_CLASSIFIER	1
#define N_GRAM_CLASSIFIER	2

#define NAIVE_BAYES_FILE_NAME	"naive_bayes_classified";
#define N_GRAM_FILE_NAME	"n_gram_classified";


using namespace std;

void printUsageAndExit()
{
  cout << "Expected usage:" << endl;
  cout << "\t./naivebayes [datasetNumber] [executionType] [number of processors]" << endl;
  cout << "Supported datasetNumber values:" << endl;
  cout << "\t" << NEWS_20 << " : 20 News Groups" << endl;
  cout << "\t" << REUTERS << " : Reuters" << endl;
  cout << "\t" << ENRON << " : Enron Emails" << endl;
  cout << "Supported executionType values:" << endl;
  cout << "\t" << SEQUENTIAL_EXECUTION << " : Sequential Execution" << endl;
  cout << "\t" << PARALLEL_EXECUTION << " : Parallel Execution" << endl;
  exit(-1);
}

int main(int argc, char* argv[])
{
  if (argc < 3)
    printUsageAndExit();
  // Select the dataset
  int datasetNumber = atoi(argv[1]);
  // Sequential or parallel?
  int executionNumber = atoi(argv[2]);
  if (executionNumber == PARALLEL_EXECUTION && argc != 4)
    printUsageAndExit();
  //
  string datasetName, vocabFileName;
  string* categoryNames;
  int categoryNames_size;
	//
  switch(datasetNumber){
    case NEWS_20:
      datasetName = NEWS_20_NAME;
      vocabFileName = "";
      break;
    case REUTERS:
      datasetName = REUTERS_NAME;
      vocabFileName = "reuters/vocab.txt";
      break;
    case ENRON:
      datasetName = ENRON_NAME;
      vocabFileName = "";
      break;
    default:
      cout << "main:: not able to select dataset" << endl;
      exit(-1);
  }
  // Initialize the NB Classifier
  string categoryNamesFile = datasetName + "category_names.txt";
  NaiveBayesClassifier nbClassifier(categoryNamesFile, vocabFileName);
  long start, runTime;
  switch (executionNumber)
  {
    case SEQUENTIAL_EXECUTION:
    {
      // nbClassifier.printAllCategoryNames();
      // How long does the training phase take?
      start = example_get_time();
      nbClassifier.learnFromTrainingSet(datasetName);
      runTime = example_get_time() - start;
      cout << "Elapsed time for sequential learning: " << runTime/1000.f << " seconds" << endl;
      // How about categorizing the whole test set?
      start = example_get_time();
      unordered_map<string, string> docClassifications;
      nbClassifier.classifyDocumentsInFile(datasetName+"test/mega_document", docClassifications);
      runTime = example_get_time() - start;
      cout << "Elapsed time for sequential classification of entire test set: " << runTime/1000.f << " seconds" << endl;
      // Print out all classifications
      // cout << "Document Classifications:" << endl;
      // typedef unordered_map<string, string>::iterator it_type;
      // for (it_type it = docClassifications.begin(); it != docClassifications.end(); it++)
      // {
      //   cout << "\t" << it->first << " " << it->second << endl;
      // }
      // Validator stuff
			categoryNames_size = nbClassifier.getCategoryCount();
			categoryNames = new string[categoryNames_size];
			nbClassifier.getCategoryNames(categoryNames);
      Validator validate(datasetNumber, NAIVE_BAYES_CLASSIFIER, categoryNames, categoryNames_size);
			start = example_get_time();
			validate.f_measure(docClassifications);
			runTime = example_get_time() - start;
			cout << "Elapsed time for sequential validation: " << runTime/1000.f << " seconds" << endl;
      break;
    }
    case PARALLEL_EXECUTION:
    {
    	// number of processors
    	int p = atoi(argv[3]);
      cout << "Using " << p << " processors" << endl;
      // Tell cilk how many processors we want
      __cilkrts_set_param((void *)"nworkers", argv[3]);
      // nbClassifier.printAllCategoryNames();
      // How long does the training phase take?
      start = example_get_time();
      nbClassifier.learnFromTrainingSetParallel(datasetName, p);
      runTime = example_get_time() - start;
      cout << "Elapsed time for parallel learning: " << runTime/1000.f << " seconds" << endl;
      // How about categorizing the whole test set?
      start = example_get_time();
      unordered_map<string, string> docClassifications = nbClassifier.classifyTestSetParallel(datasetName+"test/mega_document", p);
      runTime = example_get_time() - start;
      cout << "Elapsed time for parallel classification of entire test set: " << runTime/1000.f << " seconds" << endl;
      // Print out all classifications
      // cout << "Document Classifications:" << endl;
      // typedef unordered_map<string, string>::iterator it_type;
      // for (it_type it = docClassifications.begin(); it != docClassifications.end(); it++)
      // {
      //   cout << "\t" << it->first << " " << it->second << endl;
      // }
      categoryNames_size = nbClassifier.getCategoryCount();
      categoryNames = new string[categoryNames_size];
      nbClassifier.getCategoryNames(categoryNames);
      Validator validate(datasetNumber, NAIVE_BAYES_CLASSIFIER, categoryNames, categoryNames_size);
      start = example_get_time();
      validate.f_measure(docClassifications);
      runTime = example_get_time() - start;
      break;
    }
    default:
    {
      cout << "main::unrecognized execution type: " << executionNumber << endl;
      exit(-1);
    }
  }
  //Validator validate(NEWS_20, NAIVE_BAYES_CLASSIFIER);
  delete [] categoryNames;
  
  return 0;
}