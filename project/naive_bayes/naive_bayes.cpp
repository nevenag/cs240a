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
// #define DEBUG_4

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
  }
  else
  {
    vocabSize = readInputVocabulary(vocabFileName);
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
    // All we care about is the size
    return vocabVector.size();
}

// Private Helpers for Learning Methods

int NaiveBayesClassifier::learnForCategory(string datasetName, int categoryIndex)
{
	// get category training file name
	string categoryFileName = datasetName + "train/" + categoryProbabilities[categoryIndex]->getCategoryName();
	// open category training file
	ifstream inputFile (categoryFileName);
	// create category map for words
	unordered_map <string, int> wordCounts;
	double totalWordCount = 0;
	// wanna be P(C)
	int docCount = 0;
	// Check for success
	if (!inputFile)
	{
    // Cant do anything if we dont have the training document...
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
    // for each word update vector vocabulary per category
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
    // One doc per line
    docCount++;
	}
	// Set all the word likelihood probabilities
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
#ifdef DEBUG_4
    for (int i = 0; i < numProcs; i++)
    {
      cout << "Processor '" << i;
      cout << "' has a chunk offset of '" << categoryChunks[i].offset;
      cout << "' and chunk size of '" << categoryChunks[i].size << endl;
    }
#endif
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
      }
    }
    delete [] categoryChunks;
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

unordered_map<string, string> NaiveBayesClassifier::classifyTestSetParallel(string datasetName, int p)
{
	string fileNames[32] = {
    "parallel_test_files/00", "parallel_test_files/01",
    "parallel_test_files/02", "parallel_test_files/03",
    "parallel_test_files/04", "parallel_test_files/05",
    "parallel_test_files/06", "parallel_test_files/07",
    "parallel_test_files/08", "parallel_test_files/09",
    "parallel_test_files/10", "parallel_test_files/11",
    "parallel_test_files/12", "parallel_test_files/13",
    "parallel_test_files/14", "parallel_test_files/15",
    "parallel_test_files/16", "parallel_test_files/17",
    "parallel_test_files/18", "parallel_test_files/19",
    "parallel_test_files/20", "parallel_test_files/21",
    "parallel_test_files/22", "parallel_test_files/23",
    "parallel_test_files/24", "parallel_test_files/25",
    "parallel_test_files/26", "parallel_test_files/27",
    "parallel_test_files/28", "parallel_test_files/29",
    "parallel_test_files/30", "parallel_test_files/31"
  };
  if (p > 32)
  {
    cout << "Too many processors: " << p << endl;
  }
  vector< unordered_map<string, string> > documentClassifications;
  for (int i = 0; i < p; i++)
  {
    unordered_map<string, string> map;
    documentClassifications.push_back(map);
  }
  // cilk_for over all test files (one per processor)
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