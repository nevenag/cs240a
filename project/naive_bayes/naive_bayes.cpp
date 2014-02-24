#include <fstream>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <map>
#include "naive_bayes.hpp"
#include <regex>
#include <string>

#define MAX_NUM_CATEGORIES 200

using namespace std;

// Constructors

NaiveBayesClassifier::NaiveBayesClassifier (char *categoryFileName/*, char *vocabularyFileName*/)
{
    // First we need to get the names of all the categories
    categoryCount = 0;
    string categoryNames[MAX_NUM_CATEGORIES];
    readInputCategories(categoryFileName, categoryNames);
    // Then we can get the vocabulary and create the CategoryProbabilities objects
    categoryProbabilities = new CategoryProbabilities*[categoryCount];
    // Now create the CategoryProbabilities objects
    for (int i = 0; i < categoryCount; i++)
        categoryProbabilities[i] = new CategoryProbabilities(categoryNames[i]);
    // readInputVocabulary(vocabularyFileName, categoryNames);
}

// Constructor Helpers

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

// Learning Methods

void NaiveBayesClassifier::learnFromTrainingSet()
{
    int globalLineCount = 0;
    // for all categories 
    //CategoryProbabilities **categoryProbabilities;
    for (int i = 0; i < categoryCount; i++)
    {
	// get category name
	string categoryFileName = categoryProbabilities[i]->getCategoryName();
	// read category file
	ifstream inputFile (categoryFileName);
	// create category map for words
	std::map<string, int> wordCounts;
	double fullCount = 0;
	// wanna be P(C)
	int linecount = 0;
	// Check for success
	if (!inputFile)
	{
	    // Cant do anything if we dont have class names...
	    cout << "Unable to open class names file: " << categoryFileName << endl;
	    exit(-1);
	}
	// While there's still stuff left to read...
	while (inputFile)
	{
	    // Read one line at a time
	    string line;
	    string word;
	    getline(inputFile, line);
	    // read word by word
	    std::istringstream iss(line);
	    // for each word update vector vocabular per category
	    while (std::getline(iss, word, ' '))
	    {
		// if we have the word, increment the count
		if(wordCounts.count(word) == 1)
		{
		    wordCounts[word] = wordCounts[word] + 1;
		}  
		else //if it's not in the map yet add it
		{
		  // TODO check this -- wordCounts[word] = 1;
		  wordCounts.insert(std::pair<string,int>(word,1));
		}
		fullCount += 1;
	    }
	    linecount++;
	    
	}
	// add category 
	categoryProbabilities[i]->setProbabilitiesWithCounts(map,fullCount);    
	// and close the file
	inputFile.close();
	globalLineCount += linecount;
    }
    
    return;
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

