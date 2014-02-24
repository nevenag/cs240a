#ifndef _CATEGORY_PROBABILITIES_HPP
#define _CATEGORY_PROBABILITIES_HPP

#include <unordered_map>
#include <string>

class CategoryProbabilities
{
  public:
    CategoryProbabilities(const char*,std::string*,int);
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