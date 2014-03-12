#include <cilk/cilk.h>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <cstring>
#include "naive_bayes.hpp"

#define MAX_NUM_CATEGORIES 200

// #define DEBUG_1
// #define DEBUG_2

using namespace std;

// Constructors

NaiveBayesClassifier::NaiveBayesClassifier (string categoryFileName)
{
    // First we need to get the names of all the categories
    categoryCount = 0;
    string* categoryNames;
    categoryNames = new string[MAX_NUM_CATEGORIES];
    readInputCategories(categoryFileName, categoryNames);
    // Then we can get the vocabulary and create the CategoryProbabilities objects
    categoryProbabilities = new CategoryProbabilities*[categoryCount];
    // Now create the CategoryProbabilities objects
    for (int i = 0; i < categoryCount; i++)
        categoryProbabilities[i] = new CategoryProbabilities(categoryNames[i]);
    // readInputVocabulary(vocabularyFileName, categoryNames);
}

// Constructor Helpers

void NaiveBayesClassifier::readInputCategories(string fileName, string* categoryNames)
{
    // Open the input file for reading
    ifstream inputFile (fileName);
    // Check for success
    if (!inputFile)
    {
        // Cant do anything if we dont have class names...
        cout << "readInputCategories::Unable to open class names file: " << fileName << endl;
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

void NaiveBayesClassifier::readInputVocabulary(string fileName, string *categoryNames)
{
    // Open the input file for reading
    ifstream inputFile (fileName);
    // Check for success
    if (!inputFile)
    {
        // Cant do anything if we dont have vocabulary...
        cout << "readInputVocabulary::Unable to open vocabulary file: " << fileName << endl;
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

// Private Helpers for Learning Methods

int NaiveBayesClassifier::learnForCategory(string datasetName, int categoryIndex)
{
	// get category name
	string categoryFileName = categoryProbabilities[categoryIndex]->getCategoryName();
  categoryFileName = datasetName +"train/" + categoryFileName;
	// read category file
	ifstream inputFile (categoryFileName);
	// create category map for words
	unordered_map <string, int> wordCounts;
	double totalWordCount = 0;
	// wanna be P(C)
	int docCount = 0;
	// Check for success
	if (!inputFile)
	{
    // Cant do anything if we dont have the mega document...
    cout << "learnFromTrainingSet::Unable to open training document file: " << categoryFileName << endl;
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
  		    wordCounts[word]++;
  		}  
  		else //if it's not in the map yet add it
  		{
          wordCounts[word] = 1;
  		}
  		totalWordCount++;
    }
    docCount++;
	}
	// add category 
	categoryProbabilities[categoryIndex]->setProbabilitiesWithCounts(wordCounts, totalWordCount, docCount);
  
	// and close the file
	inputFile.close();
  return docCount;
}

// Learning Methods

void NaiveBayesClassifier::learnFromTrainingSet(string datasetName)
{
    int globalDocCount = 0;
    // for all categories 
    for (int i = 0; i < categoryCount; i++)
    {
      int docCount = learnForCategory(datasetName, i);
      globalDocCount += docCount;
    }
    for (int i = 0; i < categoryCount; i++)
    {
      categoryProbabilities[i]->setPriorProbabilityWithTotalDocCount(globalDocCount);
      //cout << "Number of docs in category '" << categoryProbabilities[i]->getCategoryName() << "': " << categoryProbabilities[i]->getDocCount() << endl;
      //cout << "Probability of category '" << categoryProbabilities[i]->getCategoryName() << "' is: " << categoryProbabilities[i]->getCategoryPriorProbability() << endl;
    }
    return;
}

void NaiveBayesClassifier::learnFromTrainingSetParallel(string datasetName, int numProcs)
{
    // Partition the categories evenly amongst all processors
    struct CategoryChunk *categoryChunks = new struct CategoryChunk[numProcs];
    int chunkSize = categoryCount / numProcs;
    int leftOver = categoryCount % numProcs;
    for (int i = 0; i < numProcs; i++)
    {
      categoryChunks[i] = CategoryChunk(0, chunkSize);
      if (leftOver > 0)
      {
        categoryChunks[i].size++;
        leftOver--;
      }
      if (i != 0)
      {
        categoryChunks[i].offset = categoryChunks[i-1].offset + categoryChunks[i-1].size;
      }
    }
    // Each processor keeps its own global doc count
    int globalDocCounts[numProcs];
    memset(globalDocCounts, 0, numProcs*sizeof(int));
    // Loop through all the categories this processor is responsible for
    cilk_for (int j = 0; j < numProcs; j++)
    {
      for (int i = categoryChunks[j].offset; i < categoryChunks[j].offset+categoryChunks[j].size; i++)
      {
        globalDocCounts[j] += learnForCategory(datasetName, i);
      }
    }
    // Sum up global doc counts
    int globalDocCount = 0;
    for (int i = 0; i < numProcs; i++)
    {
      globalDocCount += globalDocCounts[i];
    }
    // Set priors for the categories we are responsible for
    cilk_for (int j = 0; j < numProcs; j++)
    {
      for (int i = categoryChunks[j].offset; i < categoryChunks[j].offset+categoryChunks[j].size; i++)
      {
        categoryProbabilities[i]->setPriorProbabilityWithTotalDocCount(globalDocCount);
        cout << "Processor '" << j << "' is responsible for category '" << categoryProbabilities[i]->getCategoryName() << "', which has prior probability: " << categoryProbabilities[i]->getCategoryPriorProbability() << endl;
      }
    }
    delete [] categoryChunks;
}

// Document Classification Internal Helpers

string constructClassificationString(string docID, string categoryName)
{
  int bufferSize = docID.size()+categoryName.size()+4;
  char *buffer = new char[bufferSize];
  sprintf(buffer, "%s %s\n", docID.c_str(), categoryName.c_str());
  string classificationString(buffer);
  return classificationString;
}

// Document Classification
/*
  This method assumes that all test documents are placed into a single file. The results of
   the classification are written to a file called 'naive_bayes_classified' in the form of
   (documentID assignedCategory) pairs, one per line.
  @param testSetFileName: The name of the single text file with all of the test documents.
  @param docSeparator: The separator tag placed around documents. If this is equal to \n then
                       each line will be treated as a separate document.
*/
void NaiveBayesClassifier::classifyTestSet(string datasetName)
{
	// Open document file
	ifstream inputFile (datasetName+"test/mega_document");
	// Check for success
	if (!inputFile)
	{
	    // Cant do anything if we dont have documents...
	    cout << "classifyTestSet::Unable to open test set document file: " << datasetName+"test/mega_document" << endl;
	    exit(-1);
	}
  // Open output file
  ofstream outputFile (datasetName+"naive_bayes_classified");
  if (!outputFile)
  {
    cout << "classifyTestSet::Unable to open output file" << endl;
    exit(-1);
  }
  // For every document, we will need to store the P(C_j|d) terms for each C_j, initialized to
  // the prior probability, i.e. P(C_j)
  double *classificationProbabilities = new double[categoryCount];
  // String to hold docIDs
  string docID = "";
  while(inputFile)
  {
    // Grab one line at a time
    string line;
    getline(inputFile, line);
    // Each new line is a document
    // Get the docID, it will be the first word of the line
    istringstream tempISS(line);
    getline(tempISS, docID, '\t');
    // Need to reinitialize probabilities
    for (int i = 0; i < categoryCount; i++)
    {
        classificationProbabilities[i] = categoryProbabilities[i]->getCategoryPriorProbability();
    }
    // The main starting point for looping over a documents words
    istringstream iss(line);
    string word;
    while (getline(iss, word, ' '))
    {
      // Need to figure out P(w_i|C_j) for each C_j
      // where word == w_i
      for (int i = 0; i < categoryCount; i++)
      {
          classificationProbabilities[i] += categoryProbabilities[i]->getProbabilityOfWord(word);
      }
    }
    // We just finished calculating probabilities for a document so we need to classify
    // the document now...Need to find the max probability
    double maximum = classificationProbabilities[0];
    int index = 0;
#ifdef DEBUG_2
    cout << "Probability of category '" << categoryProbabilities[0]->getCategoryName() << "': " << maximum << endl;
#endif
    for (int i = 1; i < categoryCount; i++)
    {
#ifdef DEBUG_2
        cout << "Probability of category '" << categoryProbabilities[i]->getCategoryName() << "': " << classificationProbabilities[i] << endl;
#endif
        if (classificationProbabilities[i] > maximum)
        {
            maximum = classificationProbabilities[i];
            index = i;
        }
    }
    // Now we need to store this max
    string classificationString = constructClassificationString(docID,
                                                                categoryProbabilities[index]->getCategoryName());
#ifdef DEBUG_2
    cout << "About to write to output file:\n" << classificationString << endl;
#endif
    outputFile.write(classificationString.c_str(), classificationString.size());
    docID = "";
  }
}

void NaiveBayesClassifier::classifyTestSetParallel(string datasetName, int p)
{
	string fileNames[16] = {"x00", "x01", "x02", "x03", "x04", "x05", "x05", "x07",
													"x08", "x09", "x10", "x11", "x12", "x13", "x14", "x15"};

	string outputFile = datasetName+"naive_bayes_classified";
	// TODO check if opened
	std::ofstream ofs;
	ofs.open(outputFile, std::ofstream::app);
	if (!ofs)
	{
		cout << "classifyTestSetParallel::Unable to open output file" << endl;
		exit(-1);
	}
	
	// cilk_for
	for(int i = 0; i < p; i++){
		classifyTestSet(fileNames[i]);
	}
		
}

void NaiveBayesClassifier::classifyDocument(string documentFileName)
{
	// Open document file
	ifstream inputFile (documentFileName);
	// Check for success
	if (!inputFile)
	{
	    // Cant do anything if we dont have class names...
	    cout << "classifyDocument::Unable to open document file: " << documentFileName << endl;
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
                classificationProbabilities[i] += categoryProbabilities[i]->getProbabilityOfWord(word);
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
    double maximum = classificationProbabilities[0];
    int index = 0;
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

// Getters

int NaiveBayesClassifier::getCategoryNames(string *categoryNames)
{
  //categoryNames = new string[categoryCount];
  for (int i = 0; i < categoryCount; i++)
  {
      categoryNames[i] = categoryProbabilities[i]->getCategoryName();
  }
  return categoryCount;
}