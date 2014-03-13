#ifndef _VALIDATOR_HPP
#define _VALIDATOR_HPP

class Validator
{
  public:
		Validator(int, int, std::string*, int);
		void validate(std::unordered_map<std::string, std::string> &classified);
		void f_measure(std::unordered_map<std::string, std::string> &classified);
		void f_measure_parallel(std::unordered_map<std::string, std::string> &);
		void readCategorizedData(std::string, std::unordered_map<std::string, std::string> &);
		void printMatrix(int** matrix, int n);
  private:
		int dataset;
		int classifier;
		int n;
		std::string* categoryNames;
		std::string test_set_classified_name;
		std::string classified_name;
};

#endif