
#include "category_probabilities.hpp"

using namespace std;

CategoryProbabilities::CategoryProbabilities(char *cName, string *vocabulary, int vocabSize)
{
  categoryName(cName);
  // Initialize all probabilities to 0
  for (int i = 0; i < vocabSize; i++)
  {
    wordLikelihoodProbabilities[vocabulary[i]] = 0.0;
  }
}