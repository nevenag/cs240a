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
  private:
    // Private members
    CategoryProbabilities **categoryProbabilities;
    // std::string *categoryNames;
    int categoryCount;
    // Private methods
    void readInputCategories(char*,std::string*);
    void readInputVocabulary(char*,std::string*);
    void computeCategoryPriors();
    void computeWordLikelihoods();
};

#endif