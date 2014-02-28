#ifndef _CATEGORY_PROBABILITIES_HPP
#define _CATEGORY_PROBABILITIES_HPP

#include <unordered_map>
#include <string>

class CategoryProbabilities
{
  public:
    // Constructors
    CategoryProbabilities(std::string,std::string*,int);
    CategoryProbabilities(std::string);
    // Getters
    std::string getCategoryName() { return categoryName; };
    double getCategoryPriorProbability() { return categoryPriorProbability; };
    double getProbabilityOfWord(std::string);
    // Setters
    void setProbabilitiesWithCounts(std::unordered_map<std::string,int>,double,int);
    void setPriorProbabilityWithTotalDocCount(int);
    void updateCountForWord(std::string);
  private:
    // Private Members
    std::string categoryName;
    // P(C_j)
    double categoryPriorProbability;
    // Count of all documents in this category
    int docCount;
    // all P(w_i | C_j) terms stored in a hash map where
    // the key is w_i and the value is P(w_i | C_j)
    std::unordered_map <std::string, double> wordLikelihoodProbabilities;
    // If a word is not in the training set, it has some set probability
    double unknownWordProbability;
};

#endif