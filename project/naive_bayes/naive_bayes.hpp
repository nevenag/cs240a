#ifndef _NAIVE_BAYES_HPP
#define _NAIVE_BAYES_HPP

#include <string>
#include <unordered_map>
#include "category_probabilities.hpp"

struct CategoryChunk {
  int offset;
  int size;
  CategoryChunk(int o=0, int s=0):offset(o), size(s){}
};

class NaiveBayesClassifier
{
  public:
    // Constructors
    NaiveBayesClassifier(std::string);
    // Public Utility
    void printAllCategoryNames();
    // Learning
    void learnFromTrainingSet(std::string,std::string);
    void learnFromTrainingSetParallel(std::string,int);
    // Classification
    void classifyTestSet(std::string);
    void classifyTestSetParallel(std::string,int);
    void classifyDocument(std::string);
    // Getters
    CategoryProbabilities** getCategoryProbabilities() { return categoryProbabilities; };
    int getCategoryCount() { return categoryCount; };
    int getCategoryNames(std::string*);
  private:
    // Private members
    int categoryCount;
    CategoryProbabilities **categoryProbabilities;
    // Private methods
    void readInputCategories(std::string,std::string*);
    void readInputVocabulary(std::string,std::string*);
    int learnForCategory(std::string,int);
};

#endif