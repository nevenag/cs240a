#include <cilk/cilk.h>
// #include <cilktools/fake_mutex.h>
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
// #define DEBUG_3

using namespace std;

// Destructor

NaiveBayesClassifier::~NaiveBayesClassifier()
{
  for (int i = 0; i < categoryCount; i++)
    delete categoryProbabilities[i];
  delete [] categoryProbabilities;
}

// Constructors

NaiveBayesClassifier::NaiveBayesClassifier (string categoryFileName)
{
  NaiveBayesClassifier(categoryFileName, "");
}

NaiveBayesClassifier::NaiveBayesClassifier (string categoryFileName, string vocabFileName)
{
  // Check for vocab file
  int vocabSize;
  if (vocabFileName.compare("") == 0)
  {
    vocabSize = 0;
    // cout << "No vocab file" << endl;
  }
  else
  {
    vocabSize = readInputVocabulary(vocabFileName);
    // cout << "Vocab size: " << vocabSize << endl;
  }
  // First we need to get the names of all the categories
  categoryCount = 0;
  string* categoryNames = new string[MAX_NUM_CATEGORIES];
  readInputCategories(categoryFileName, categoryNames);
  // Now create the CategoryProbabilities objects
  categoryProbabilities = new CategoryProbabilities*[categoryCount];
  for (int i = 0; i < categoryCount; i++)
    categoryProbabilities[i] = new CategoryProbabilities(categoryNames[i], vocabSize);
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

int NaiveBayesClassifier::readInputVocabulary(string fileName)
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
    return vocabVector.size();
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
    int *globalDocCounts = new int[numProcs]();
    // Loop through all the categories this processor is responsible for
    // cilkscreen::fake_mutex m;
    cilk_for (int j = 0; j < numProcs; j++)
    {
      for (int i = categoryChunks[j].offset; i < categoryChunks[j].offset+categoryChunks[j].size; i++)
      {
        // m.lock();
        globalDocCounts[j] += learnForCategory(datasetName, i);
        // m.unlock();
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
        // cout << "Processor '" << j << "' is responsible for category '" << categoryProbabilities[i]->getCategoryName() << "', which has prior probability: " << categoryProbabilities[i]->getCategoryPriorProbability() << endl;
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

void NaiveBayesClassifier::classifyDocumentsInFile(string documentFileName, unordered_map<string, string> &documentClassifications)
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
  // For every document, we will need to store the P(C_j|d) terms for each C_j, initialized to
  // the prior probability, i.e. P(C_j)
  double *classificationProbabilities = new double[categoryCount];
  // String to hold docIDs
  string docID = "";
	// While there's still stuff left to read...
	while (inputFile)
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
#ifdef DEBUG_2
    cout << "About to add entry to unordered_map:" << docID << ":" << categoryProbabilities[index]->getCategoryName() << endl;
#endif
    documentClassifications[docID] = categoryProbabilities[index]->getCategoryName();
    docID = "";
  }
  // Done with the document file now
  inputFile.close();
  delete [] classificationProbabilities;
#ifdef DEBUG_3
  cout << "Classified " << documentClassifications.size() << " number of documents" << endl;
#endif
}

/*
  This method assumes that all test documents are placed into a single file. The results of
   the classification are written to a file called 'naive_bayes_classified' in the form of
   (documentID assignedCategory) pairs, one per line.
  @param testSetFileName: The name of the single text file with all of the test documents.
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
  
}

unordered_map<string, string> NaiveBayesClassifier::classifyTestSetParallel(string datasetName, int p)
{
	string fileNames[16] = {"x00", "x01", "x02", "x03", "x04", "x05", "x06", "x07",
													"x08", "x09", "x10", "x11", "x12", "x13", "x14", "x15"};
  if (p > 16)
  {
    cout << "Too many processors: " << p << endl;
  }
  vector< unordered_map<string, string> > documentClassifications;
  for (int i = 0; i < p; i++)
  {
    unordered_map<string, string> map;
    documentClassifications.push_back(map);
  }
  // cilk_for
	cilk_for (int i = 0; i < p; i++)
  {
		classifyDocumentsInFile(fileNames[i], documentClassifications[i]);
	}
  for (int i = 1; i < p; i++)
  {
#ifdef DEBUG_3
  cout << "documentClassifications[0].size() = " << documentClassifications[0].size() << endl;
  cout << "documentClassifications[i].size() = " << documentClassifications[i].size() << endl;
#endif
    documentClassifications[0].insert(documentClassifications[i].begin(), documentClassifications[i].end());
  }
#ifdef DEBUG_3
  cout << "Parallel:Classified " << documentClassifications[0].size() << " number of documents" << endl;
#endif
	return documentClassifications[0];
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