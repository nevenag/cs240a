#include <iostream>

#include "naive_bayes.hpp"

using namespace std;

int main()
{
  cout << "Main" << endl;
  
  NaiveBayesClassifier nbClassifier("all-topics-strings.lc.txt");
  nbClassifier.printAllClassNames();
  
  return 0;
}