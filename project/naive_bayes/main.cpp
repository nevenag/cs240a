#include <iostream>
#include <ctime>

#include "naive_bayes.hpp"

using namespace std;

int main()
{
  cout << "Main" << endl;
  // Initialize the NB Classifier
  char categoryNamesFile[] = "category_names.txt";
  NaiveBayesClassifier nbClassifier(categoryNamesFile);
  nbClassifier.printAllCategoryNames();
  // How long does the training phase take?
  clock_t start = clock();
  nbClassifier.learnFromTrainingSet();
  clock_t runTime = clock() - start;
  cout << "Elapsed time for learning: " << (double)runTime/CLOCKS_PER_SEC << " seconds" << endl;
  // How quickly can we categorize a single docuemnt?
  char documentToClassify[] = "categorize.me";
  start = clock();
  nbClassifier.classifyDocument(documentToClassify);
  runTime = clock() - start;
  cout << "Elapsed time for classification: " << (double)runTime/CLOCKS_PER_SEC << " seconds" << endl;
  
  return 0;
}