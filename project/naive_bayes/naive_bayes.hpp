#ifndef _NAIVE_BAYES_HPP
#define _NAIVE_BAYES_HPP

#include <string>

#include "category_probabilities.hpp"

class NaiveBayesClassifier
{
  public:
    NaiveBayesClassifier(char*,char*);
    void printAllCategoryNames();
    void learnFromTrainingSet(char**);
  private:
    // Private members
    CategoryProbabilities *categoryProbabilities;
    // std::string *categoryNames;
    int categoryCount;
    // Private methods
    void readInputVocabulary(char*);
    void readInputCategories(char*);
    void computeCategoryPriors();
    void computeWordLikelihoods();
};

#endif