#include <fstream>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <sstream>
#include "naive_bayes.hpp"

#define MAX_NUM_CATEGORIES 200

//#define DEBUG_1

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
        if (line != "")
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
    for (int i = 0; i < categoryCount; i++)
    {
    	// get category name
    	string categoryFileName = categoryProbabilities[i]->getCategoryName();
    	// read category file
    	ifstream inputFile (categoryFileName);
    	// create category map for words
    	unordered_map <string, int> wordCounts;
    	double fullCount = 0;
    	// wanna be P(C)
    	int lineCount = 0;
    	// Check for success
    	if (!inputFile)
    	{
    	    // Cant do anything if we dont have the mega document...
    	    cout << "Unable to open training document file: " << categoryFileName << endl;
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
    	    istringstream iss(line);
    	    // for each word update vector vocabular per category
    	    while (getline(iss, word, ' '))
    	    {
        		// if we have the word, increment the count
        		if(wordCounts.count(word) == 1)
        		{
        		    wordCounts[word]++;// = wordCounts[word] + 1;
        		}  
        		else //if it's not in the map yet add it
        		{
        		  //TODO: check this
                  wordCounts[word] = 1;
        		  //wordCounts.insert(pair<string,int>(word,1));
        		}
        		fullCount++;
    	    }
    	    lineCount++;
	    
    	}
    	// add category 
    	categoryProbabilities[i]->setProbabilitiesWithCounts(wordCounts, fullCount, lineCount);    
    	// and close the file
    	inputFile.close();
    	globalLineCount += lineCount;
    }
    for (int i = 0; i < categoryCount; i++)
    {
        categoryProbabilities[i]->setPriorProbabilityWithTotalDocCount(globalLineCount);
    }
    return;
}

// Document Classification

void NaiveBayesClassifier::classifyDocument(char *documentFileName)
{
	// Open document file
	ifstream inputFile (documentFileName);
	// Check for success
	if (!inputFile)
	{
	    // Cant do anything if we dont have class names...
	    cout << "Unable to open document file: " << documentFileName << endl;
	    exit(-1);
	}
    // Ok so now we have an open file that represents the document we want to classify
    // We need to be able to store the P(C_j|d) terms for each C_j, initialized to
    // the prior probability, i.e. P(C_j)
    double *classificationProbabilities = new double[categoryCount];
    double sum = 0.0;
    for (int i = 0; i < categoryCount; i++)
    {
        classificationProbabilities[i] = categoryProbabilities[i]->getCategoryPriorProbability();
        sum += classificationProbabilities[i];
    }
#ifdef DEBUG_1
    int *probabilityIsZero = new int[categoryCount]();
#endif
	// While there's still stuff left to read...
	while (inputFile)
	{
	    // Read one line at a time
	    string line;
        getline(inputFile, line);
	    // Read each line word by word
	    istringstream iss(line);
        string word;
	    while (getline(iss, word, ' '))
	    {
            // Need to figure out P(w_i|C_j) for each C_j
            // where word == w_i
            for (int i = 0; i < categoryCount; i++)
            {
                classificationProbabilities[i] *= categoryProbabilities[i]->getProbabilityOfWord(word);
#ifdef DEBUG_1
                if (categoryProbabilities[i]->getProbabilityOfWord(word) == 0.0)
                {
                    cout << "Probability of word '" << word << "' for category '" << categoryProbabilities[i]->getCategoryName() << "' is zero" << endl;
                }
                if (classificationProbabilities[i] == 0.0 && probabilityIsZero[i] == 0)
                {
                    cout << "Probability of category '" << categoryProbabilities[i]->getCategoryName() << "' became zero with word '" << word << "' which had probability: " << categoryProbabilities[i]->getProbabilityOfWord(word) << endl;
                    probabilityIsZero[i] = 1;
                }
#endif
            }
        }
    }
    // Done with the document file now
    inputFile.close();
    // Need to find the max
    double maximum = 0.0;
    int index = -1;
    for (int i = 0; i < categoryCount; i++)
    {
#ifdef DEBUG_1
        cout << "Is " << classificationProbabilities[i] << " greater than zero?" << endl;
#endif
        if (classificationProbabilities[i] > maximum)
        {
            maximum = classificationProbabilities[i];
            index = i;
        }
    }
#ifdef DEBUG_1
    cout << "Index: " << index << endl;
#endif
    cout << "The document specified by " << documentFileName << " belongs to the " << categoryProbabilities[index]->getCategoryName() << " category." << endl;
    delete [] classificationProbabilities;
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

