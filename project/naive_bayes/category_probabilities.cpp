#include <iostream>
#include "category_probabilities.hpp"

#define SCALING_FACTOR 10000.0
#define ALPHA 1.0

using namespace std;

// Constructors

CategoryProbabilities::CategoryProbabilities(string cName, int vocabSize)
{
  categoryName = cName;
  this->vocabSize = vocabSize;
  cout << "Using vocab size of: " << vocabSize << endl;
}

CategoryProbabilities::CategoryProbabilities(string cName)
{
    categoryName = cName;
    vocabSize = 0;
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
  // categoryPriorProbability = SCALING_FACTOR * ((double)docCount / totalDocCount);
  categoryPriorProbability = log(((double)docCount / totalDocCount));
}

void CategoryProbabilities::setProbabilitiesWithCounts(unordered_map <string, int> wordCounts, double totalWordCount, int docCount)
{
    this->docCount = docCount;
    // Vocab size + 1 for unknown word
    int vocabSize = /*this->vocabSize == 0 ? */wordCounts.size() + 1;// : this->vocabSize;
    typedef unordered_map<string, int>::iterator it_type;
    for (it_type iterator = wordCounts.begin(); iterator != wordCounts.end(); iterator++)
    {
      // add-one smoothing
      wordLikelihoodProbabilities[iterator->first] = log(( (iterator->second + ALPHA) / (totalWordCount + ALPHA*vocabSize) ));
        // wordLikelihoodProbabilities[iterator->first] = SCALING_FACTOR*(iterator->second + ALPHA) / (totalWordCount + ALPHA*vocabSize);
    }
    // Unknown words have a fixed probability according to add-alpha smoothing
    unknownWordProbability = log((ALPHA / (totalWordCount + ALPHA*vocabSize)));
    // unknownWordProbability = SCALING_FACTOR*ALPHA / (totalWordCount + ALPHA*vocabSize);
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