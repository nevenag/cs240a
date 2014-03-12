#include "mpi.h"
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
  if (argc != 4)
    printUsageAndExit();
  // Select the dataset
  int datasetNumber = atoi(argv[1]);
  string datasetName, testSetSeparator;
  string* categoryNames;
  int categoryNames_size;
	// number of processors
	int p = atoi(argv[3]);

  switch(datasetNumber){
    case NEWS_20:
      datasetName = NEWS_20_NAME;
      testSetSeparator = "\n";
      break;
    case REUTERS:
      datasetName = REUTERS_NAME;
      testSetSeparator = "<BODY>";
      break;
    case ENRON:
      datasetName = ENRON_NAME;
      break;
    default:
      cout << "main:: not able to select dataset" << endl;
      exit(-1);
  }
  // Initialize the NB Classifier
  string categoryNamesFile = datasetName + "category_names.txt";
  NaiveBayesClassifier nbClassifier(categoryNamesFile);
  // Select parallel or sequential
  int executionNumber = atoi(argv[2]);
  clock_t start, runTime;
  switch (executionNumber)
  {
    case SEQUENTIAL_EXECUTION:
    {
      nbClassifier.printAllCategoryNames();
      // How long does the training phase take?
      start = clock();
      nbClassifier.learnFromTrainingSet(datasetName, testSetSeparator);
      runTime = clock() - start;
      cout << "Elapsed time for sequential learning: " << (double)runTime/CLOCKS_PER_SEC << " seconds" << endl;
      // How quickly can we categorize a single docuemnt?
      string documentToClassify = "categorize.me";
      start = clock();
      nbClassifier.classifyDocument(documentToClassify);
      runTime = clock() - start;
      cout << "Elapsed time for sequential classification: " << (double)runTime/CLOCKS_PER_SEC << " seconds" << endl;
      // How about a whole test set
      start = clock();
      nbClassifier.classifyTestSet(datasetName, testSetSeparator);
      runTime = clock() - start;
      cout << "Elapsed time for sequential classification of entire test set: " << (double)runTime/CLOCKS_PER_SEC << " seconds" << endl;
      break;
    }
    case PARALLEL_EXECUTION:
    {
      MPI_Init(&argc, &argv);
      int size, rank;
      MPI_Comm_size(MPI_COMM_WORLD, &size);
      MPI_Comm_rank(MPI_COMM_WORLD, &rank);
      if (rank == 0)
        nbClassifier.printAllCategoryNames();
      // How long does the training phase take?
      start = clock();
      nbClassifier.learnFromTrainingSetParallel(datasetName, testSetSeparator, size, rank);
      runTime = clock() - start;
      if (rank == 0)
        cout << "Elapsed time for parallel learning: " << (double)runTime/CLOCKS_PER_SEC << " seconds" << endl;
      MPI_Finalize();
      break;
    }
    default:
    {
      cout << "main::unrecognized execution type: " << executionNumber << endl;
      exit(-1);
    }
  }
	
	categoryNames_size = nbClassifier.getCategoryCount();
	categoryNames = new string[categoryNames_size];
	nbClassifier.getCategoryNames(categoryNames);
  Validator validate(NEWS_20, NAIVE_BAYES_CLASSIFIER, categoryNames, categoryNames_size);
  //Validator validate(NEWS_20, NAIVE_BAYES_CLASSIFIER);
  delete [] categoryNames;
  
  return 0;
}