#ifndef _CATEGORY_PROBABILITIES_HPP
#define _CATEGORY_PROBABILITIES_HPP

#include <unordered_map>
#include <string>

class CategoryProbabilities
{
  public:
    CategoryProbabilities(std::string,std::string*,int);
    CategoryProbabilities(std::string);
    std::string getCategoryName() { return categoryName; };
    void setProbabilitiesWithCounts(std::unordered_map <std::string, int>,double);
    void updateCountForWord(std::string);
  private:
    // Private Members
    std::string categoryName;
    // P(C_j)
    double categoryPriorProbability;
    // all P(w_i | C_j) terms stored in a hash map where
    // the key is w_i and the value is P(w_i | C_j)
    std::unordered_map <std::string, double> wordLikelihoodProbabilities;
};

#endif