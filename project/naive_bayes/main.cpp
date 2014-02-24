#include <iostream>

#include "naive_bayes.hpp"

using namespace std;

int main()
{
  cout << "Main" << endl;
  
  char categoryNamesFile[] = "all-topics-strings.lc.txt";
  NaiveBayesClassifier nbClassifier(categoryNamesFile);
  nbClassifier.printAllCategoryNames();
  
  return 0;
}