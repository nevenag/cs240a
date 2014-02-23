#ifndef _NAIVE_BAYES_HPP
#define _NAIVE_BAYES_HPP

#include <string>

class NaiveBayesClassifier
{
  public:
    NaiveBayesClassifier(char*);
    
  private:
    // Private members
    std::string *classNames;
    int classCount;
    // Private methods
    void readInputClasses(char*);
};

#endif