
#include "category_probabilities.hpp"

using namespace std;

// Constructors

CategoryProbabilities::CategoryProbabilities(string cName, string *vocabulary, int vocabSize)
{
  categoryName = cName;
  // Initialize all probabilities to 0
  for (int i = 0; i < vocabSize; i++)
  {
    wordLikelihoodProbabilities[vocabulary[i]] = 0.0;
  }
}

CategoryProbabilities::CategoryProbabilities(string cName)
{
    categoryName = cName;
}

// 

void CategoryProbabilities::setProbabilitiesWithCounts(unordered_map <string, int> wordCounts, double totalWordCount)
{
    for (auto wordCountPair : wordCounts)
    {
        wordLikelihoodProbabilities[wordCountPair.first] = wordCountPair.second / totalWordCount;
    }
}

void CategoryProbabilities::updateCountForWord(string word)
{
    if (wordLikelihoodProbabilities.count(word) == 1)
    {
        wordLikelihoodProbabilities[word] += 1.0;
    }
    else
    {
        wordLikelihoodProbabilities[word] = 1.0;
    }
}