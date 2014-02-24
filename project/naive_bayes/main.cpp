#include <iostream>

#include "naive_bayes.hpp"

using namespace std;

int main()
{
  cout << "Main" << endl;
  
  char classNamesFile[] = "all-topics-strings.lc.txt";
  NaiveBayesClassifier nbClassifier(classNamesFile);
  nbClassifier.printAllClassNames();
  
  return 0;
}