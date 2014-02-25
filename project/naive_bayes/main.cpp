#include <iostream>

#include "naive_bayes.hpp"

using namespace std;

int main()
{
  cout << "Main" << endl;
  
  char categoryNamesFile[] = "category_names.txt";
  // char vocabularyFile[] = "all-topics-strings.lc.txt";
  NaiveBayesClassifier nbClassifier(categoryNamesFile/*, vocabularyFile*/);
  nbClassifier.printAllCategoryNames();
  nbClassifier.learnFromTrainingSet();
  char documentToClassify[] = "categorize.me";
  nbClassifier.classifyDocument(documentToClassify);
  
  return 0;
}