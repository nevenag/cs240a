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
    void learnFromTrainingSet(std::string,std::string);
    void learnFromTrainingSetParallel(std::string,std::string,int,int);
    void classifyTestSet(std::string,std::string);
    void classifyDocument(std::string);
    CategoryProbabilities** getCategoryProbabilities() { return categoryProbabilities; };
    int getCategoryCount() { return categoryCount; };
  private:
    // Private members
    int categoryCount;
    CategoryProbabilities **categoryProbabilities;
    // Private methods
    void readInputCategories(std::string,std::string*);
    void readInputVocabulary(std::string,std::string*);
    int learnForCategory(std::string,std::string,std::string,std::string,int);
};

#endif