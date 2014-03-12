#ifndef _VALIDATOR_HPP
#define _VALIDATOR_HPP

class Validator
{
  public:
    Validator(int, int, std::string*, int);
		void validate(std::string classified_name, std::string test_set_classified_name);
		void f_measure(std::string classified_name, std::string test_set_classified_name, std::string*, int);
		void f_measure_parallel(std::string classified_name, std::string test_set_classified_name,std::string* categoryNames, int n);
		void readCategorizedData(std::string fileName, std::unordered_map<std::string, std::string> &classified);
		void printMatrix(int** matrix, int n);
  private:
		int dataset;
		int classifier;	
};

#endif