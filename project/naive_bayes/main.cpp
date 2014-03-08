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


using namespace std;


int main()
{
  // Select the dataset
  int datasetNumber;
  string datasetName, testSetSeparator;
  cout << "main::select dataset" << endl;
  cout << "1 for 20news" << endl;
  cout << "2 for reuters" << endl;
  cout << "3 for enron" << endl;
  cin >> datasetNumber;
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
      break;
  }
  
  // Initialize the NB Classifier
  string categoryNamesFile = datasetName + "category_names.txt";
  NaiveBayesClassifier nbClassifier(categoryNamesFile);
  nbClassifier.printAllCategoryNames();
  // How long does the training phase take?
  clock_t start = clock();
  nbClassifier.learnFromTrainingSet(datasetName, testSetSeparator);
  clock_t runTime = clock() - start;
  cout << "Elapsed time for learning: " << (double)runTime/CLOCKS_PER_SEC << " seconds" << endl;
  // How quickly can we categorize a single docuemnt?
  string documentToClassify = "categorize.me";
  start = clock();
  nbClassifier.classifyDocument(documentToClassify);
  runTime = clock() - start;
  cout << "Elapsed time for classification: " << (double)runTime/CLOCKS_PER_SEC << " seconds" << endl;
  
  // How about a whole test set
  string sampleTestSetFile = datasetName + "test/mega_document";
  start = clock();
  nbClassifier.classifyTestSet(datasetName, testSetSeparator);
  runTime = clock() - start;
  cout << "Elapsed time for classification of entire test set: " << (double)runTime/CLOCKS_PER_SEC << " seconds" << endl;
  
  Validator validate(ENRON, NAIVE_BAYES_CLASSIFIER);
  //Validator validate(NEWS_20, NAIVE_BAYES_CLASSIFIER);

  return 0;
}