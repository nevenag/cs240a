#include <fstream>
#include <iostream>
#include <cstdlib>

#include "naive_bayes.hpp"

#define MAX_NUM_CATEGORIES 200

using namespace std;

// Constructors

NaiveBayesClassifier::NaiveBayesClassifier (char *categoryFileName)
{
    categoryCount = 0;
    categoryNames = new string[MAX_NUM_CATEGORIES];
    readInputCategories(categoryFileName);
}

// Constructor Helpers

void NaiveBayesClassifier::readInputCategories(char *fileName)
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
        cout << "\t" << categoryNames[i] << endl;
    }
}

