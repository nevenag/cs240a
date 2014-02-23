#include <fstream>
#include <iostream>
#include <cstdlib>

#include "naive_bayes.hpp"

#define MAX_NUM_CLASSES 200

using namespace std;

NaiveBayesClassifier::NaiveBayesClassifier (char *classFileName)
{
    classCount = 0;
    classNames = new string[MAX_NUM_CLASSES];
    readInputClasses(classFileName);
}

void NaiveBayesClassifier::readInputClasses(char *fileName)
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
        classNames[classCount++] = line;
    }
    // All done
    inputFile.close();
    return;
}

void NaiveBayesClassifier::printAllClassNames()
{
    cout << "All valid class names are as follows (" << classCount << " total):" << endl;
    for (int i = 0; i < classCount; i++)
    {
        cout << "\t" << classNames[i] << endl;
    }
}