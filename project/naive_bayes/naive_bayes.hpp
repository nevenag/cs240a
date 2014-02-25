#ifndef _NAIVE_BAYES_HPP
#define _NAIVE_BAYES_HPP

#include <string>
#include <unordered_map>
#include "category_probabilities.hpp"

class NaiveBayesClassifier
{
  public:
    NaiveBayesClassifier(char* /*,char* */);
    void printAllCategoryNames();
    void learnFromTrainingSet();
    void classifyDocument(char*);
  private:
    // Private members
    int categoryCount;
    CategoryProbabilities **categoryProbabilities;
    // Private methods
    void readInputCategories(char*,std::string*);
    void readInputVocabulary(char*,std::string*);
};

#endif