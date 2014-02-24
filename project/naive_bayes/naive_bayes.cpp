#include <fstream>
#include <iostream>
#include <cstdlib>
#include <vector>

#include "naive_bayes.hpp"

#define MAX_NUM_CATEGORIES 200

using namespace std;

// Constructors

NaiveBayesClassifier::NaiveBayesClassifier (char *categoryFileName, char *vocabularyFileName)
{
    // First we need to get the names of all the categories
    categoryCount = 0;
    string categoryNames[MAX_NUM_CATEGORIES];
    readInputCategories(categoryFileName, categoryNames);
    // Then we can get the vocabulary and create the CategoryProbabilities objects
    categoryProbabilities = new CategoryProbabilities*[categoryCount];
    readInputVocabulary(vocabularyFileName, categoryNames);
}

// Constructor Helpers

void NaiveBayesClassifier::readInputVocabulary(char *fileName, string *categoryNames)
{
    // Open the input file for reading
    ifstream inputFile (fileName);
    // Check for success
    if (!inputFile)
    {
        // Cant do anything if we dont have vocabulary...
        cout << "Unable to open vocabulary file: " << fileName << endl;
        exit(-1);
    }
    vector<string> vocabVector;
    // While there's still stuff left to read...
    while (inputFile)
    {
        // Read one line at a time
        string line;
        getline(inputFile, line);
        vocabVector.push_back(line);
    }
    // All done
    inputFile.close();
    // Now create the CategoryProbabilities objects
    for (int i = 0; i < categoryCount; i++)
        categoryProbabilities[i] = new CategoryProbabilities(categoryNames[i], &vocabVector[0], vocabVector.size());
    return;
}

void NaiveBayesClassifier::readInputCategories(char *fileName, string *categoryNames)
{
    // Open the input file for reading
    ifstream inputFile (fileName);
    // Check for success
    if (!inputFile)
    {
        // Cant do anything if we dont have class names...
        cout << "Unable to open class names file: " << fileName << endl;
        exit(-1);
    }
    // While there's still stuff left to read...
    while (inputFile)
    {
        // Read one line at a time
        string line;
        getline(inputFile, line);
        categoryNames[categoryCount++] = line;
    }
    // All done
    inputFile.close();
    return;
}

// Learning Methods

void NaiveBayesClassifier::learnFromTrainingSet(char **documentFileNames)
{
    
}
/*
    Compute all P(C_j) terms
*/
void NaiveBayesClassifier::computeCategoryPriors()
{
    
}
/*
    Compute all P(w_i | C_j) terms
*/
void NaiveBayesClassifier::computeWordLikelihoods()
{
    
}

// Public Utility Methods

void NaiveBayesClassifier::printAllCategoryNames()
{
    cout << "All valid category names are as follows (" << categoryCount << " total):" << endl;
    for (int i = 0; i < categoryCount; i++)
    {
        cout << "\t" << categoryProbabilities[i]->getCategoryName() << endl;
    }
}

