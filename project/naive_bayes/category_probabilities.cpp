#include "category_probabilities.hpp"

#define SCALING_FACTOR 10000.0
#define ALPHA 1.0

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

// Getters

double CategoryProbabilities::getProbabilityOfWord(string word)
{
    // Either we have a probability for the word
    if (wordLikelihoodProbabilities.count(word) == 1)
    {
        // This case is easy
        return wordLikelihoodProbabilities[word];
    }
    // Or its an unknown word
    else
    {
        return unknownWordProbability;
    }
}

// Setters

void CategoryProbabilities::setPriorProbabilityWithTotalDocCount(int totalDocCount)
{
  categoryPriorProbability = SCALING_FACTOR * ((double)docCount / totalDocCount);
    // categoryPriorProbability = log(((double)docCount / totalDocCount));
}

void CategoryProbabilities::setProbabilitiesWithCounts(unordered_map <string, int> wordCounts, double totalWordCount, int docCount)
{
    this->docCount = docCount;
    // Vocab size + 1 for unknown word
    int vocabSize = wordCounts.size() + 1;
    for (auto wordCountPair : wordCounts)
    {
        // add-one smoothing
        // wordLikelihoodProbabilities[wordCountPair.first] = log(( (wordCountPair.second + ALPHA) / (totalWordCount + ALPHA*vocabSize) ));
      wordLikelihoodProbabilities[wordCountPair.first] = SCALING_FACTOR*(wordCountPair.second + ALPHA) / (totalWordCount + ALPHA*vocabSize);
    }
    // Unknown words have a fixed probability according to add-alpha smoothing
    // unknownWordProbability = log((ALPHA / (totalWordCount + ALPHA*vocabSize)));
    unknownWordProbability = SCALING_FACTOR*ALPHA / (totalWordCount + ALPHA*vocabSize);
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