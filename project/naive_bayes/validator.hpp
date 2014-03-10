#ifndef _VALIDATOR_HPP
#define _VALIDATOR_HPP



class Validator
{
  public:
    Validator(int, int, std::string*, int);
	void validate(int dataset, int classifier);
	void f_measure(std::string*, int);
  private:
	int dataset;
	int classifier;	
};

#endif