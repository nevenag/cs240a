#include <fstream>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <regex>
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

// Learning Methods

void NaiveBayesClassifier::learnFromTrainingSet(string datasetName, string docSeparator)
{
    int globalDocCount = 0;
    // Only used if there are tags enclosing documents
    string openTag = "";
    string closeTag = "";
    if (regex_match(docSeparator, regex("<[A-Z]+>")))
    {
      string separatorText = docSeparator;
      separatorText.erase(0, 1);
      separatorText.erase(separatorText.end()-1);
      // Create close tag
      int buffSize = 4+separatorText.size();
      char buff[buffSize];
      sprintf(buff, "</%s>", separatorText.c_str());
      closeTag = buff;
      // Create open tag
      memset(buff, 0, buffSize);
      sprintf(buff, "<%s", separatorText.c_str());
      openTag = buff;
    }
    // for all categories 
    for (int i = 0; i < categoryCount; i++)
    {
    	// get category name
    	string categoryFileName = categoryProbabilities[i]->getCategoryName();
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
          if (docSeparator.compare("\n") == 0)
          {
            docCount++;
          }
          else if (regex_match(docSeparator, regex("<[A-Z]+>")))
          {
            if (line.compare(0, openTag.size(), openTag) == 0)
            {
              // Just ignore the document dividers
              continue;
            }
            else if (line.compare(closeTag) == 0)
            {
              // Update docCount for close tags
              docCount++;
              continue;
            }
          }
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
    	}
    	// add category 
    	categoryProbabilities[i]->setProbabilitiesWithCounts(wordCounts, totalWordCount, docCount);
      
    	// and close the file
    	inputFile.close();
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
void NaiveBayesClassifier::classifyTestSet(string datasetName, string docSeparator)
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
  // Boolean flag used to determine whether or not we are parsing a document
  bool isClassifyingDocument = false;
  string docID = "";
  string openTag = "";
  string closeTag = "";
  if (regex_match(docSeparator, regex("<[A-Z]+>")))
  {
    string separatorText = docSeparator;
    separatorText.erase(0, 1);
    separatorText.erase(separatorText.end()-1);
    // Create close tag
    int buffSize = 4+separatorText.size();
    char buff[buffSize];
    sprintf(buff, "</%s>", separatorText.c_str());
    closeTag = buff;
    // Create open tag
    memset(buff, 0, buffSize);
    sprintf(buff, "<%s", separatorText.c_str());
    openTag = buff;
  }
  while(inputFile)
  {
    // Grab one line at a time
    string line;
    getline(inputFile, line);
    // The docSeparator is used to determine when to update the boolean flag isClassifyingDocument
    if (docSeparator.compare("\n") == 0)
    {
      // Then each new line is a document
      isClassifyingDocument = true;
      // Get the docID, it will be the first word of the line
	    istringstream tempISS(line);
	    getline(tempISS, docID, '\t');
      // Need to reinitialize probabilities
      for (int i = 0; i < categoryCount; i++)
      {
          classificationProbabilities[i] = categoryProbabilities[i]->getCategoryPriorProbability();
      }
    }
    // Any separators consisting of only capital letters enclosed with <> will be treated the same
    else if (regex_match(docSeparator, regex("<[A-Z]+>")))
    {
      if (isClassifyingDocument)
      {
        // Check for close tag
        if (line.compare(closeTag) == 0)
        {
          // Ok we need to classify the document now...
          // Need to find the max probability
          double maximum = classificationProbabilities[0];
          int index = 0;
          for (int i = 1; i < categoryCount; i++)
          {
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
          // Update boolean flag and continue to next iteration of loop
          isClassifyingDocument = false;
          continue;
        }
      }
      else
      {
        // Check for open tag
        if (line.compare(0, openTag.size(), openTag) == 0)
        {
          // Need to grab the document ID, assume line is in form '<separatorText DOCID=docID>'
          vector<string> elems;
          stringstream ss(line);
          getline(ss, docID, '=');
          getline(ss, docID, '=');
          docID.erase(docID.end()-1);
          // Need to reinitialize probabilities
          for (int i = 0; i < categoryCount; i++)
          {
              classificationProbabilities[i] = categoryProbabilities[i]->getCategoryPriorProbability();
          }
          // Update boolean flag and continue to next iteration of loop
          isClassifyingDocument = true;
          continue;
        }
      }
    }
    else
    {
      cout << "Unsupported document separator: " << docSeparator << endl;
      exit(-1);
    }
    // The main entry point for looping over a documents words
    if (isClassifyingDocument)
    {
	    istringstream iss(line);
      string word;
	    while (getline(iss, word, ' '))
	    {
        // Need to figure out P(w_i|C_j) for each C_j
        // where word == w_i
        for (int i = 0; i < categoryCount; i++)
        {
            classificationProbabilities[i] *= categoryProbabilities[i]->getProbabilityOfWord(word);
        }
      }
      // For new line separator, the end of this while loop is the end of the doc
      if (docSeparator.compare("\n") == 0)
      {
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

