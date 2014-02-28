#ifndef _NAIVE_BAYES_HPP
#define _NAIVE_BAYES_HPP

#include <string>
#include <unordered_map>
#include "category_probabilities.hpp"

class NaiveBayesClassifier
{
  public:
    NaiveBayesClassifier(std::string);
    void printAllCategoryNames();
    void learnFromTrainingSet(std::string);
    void classifyDocument(std::string);
  private:
    // Private members
    int categoryCount;
    CategoryProbabilities **categoryProbabilities;
    // Private methods
    void readInputCategories(std::string,std::string*);
    void readInputVocabulary(std::string,std::string*);
};

#endif