#ifndef _NAIVE_BAYES_HPP
#define _NAIVE_BAYES_HPP

#include <string>

class NaiveBayesClassifier
{
  public:
    NaiveBayesClassifier(char*);
    void printAllCategoryNames();
    void learnFromTrainingSet(char**);
  private:
    // Private members
    std::string *categoryNames;
    int categoryCount;
    // Private methods
    void readInputCategories(char*);
    void computeCategoryPriors();
    void computeWordLikelihoods();
};

#endif