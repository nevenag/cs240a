#include <iostream>
#include <ctime>

#include "naive_bayes.hpp"

using namespace std;

int main()
{
  int datasetNumber;
  string datasetName;
  cout << "main::select dataset" << endl;
  cout << "1 for 20news" << endl;
  cout << "2 for reuters" << endl;
  cout << "3 for enron" << endl;
  cin >> datasetNumber;
  switch(datasetNumber){
    case 1:
      datasetName = "./20news/";
      break;
    case 2:
      datasetName = "";
      break;
    case 3:
      datasetName = "";
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
  nbClassifier.learnFromTrainingSet(datasetName);
  clock_t runTime = clock() - start;
  cout << "Elapsed time for learning: " << (double)runTime/CLOCKS_PER_SEC << " seconds" << endl;
  // How quickly can we categorize a single docuemnt?
  string documentToClassify = "categorize.me";
  start = clock();
  nbClassifier.classifyDocument(documentToClassify);
  runTime = clock() - start;
  cout << "Elapsed time for classification: " << (double)runTime/CLOCKS_PER_SEC << " seconds" << endl;
  
  return 0;
}