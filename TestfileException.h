/*
 * TestfileException.h
 *
 *  Created on: 26.11.2015
 *      Author: CAT
 */

#ifndef TESTFILEEXCEPTION_H_
#define TESTFILEEXCEPTION_H_

#include <exception>
#include <string>

/**
 * This class is used to identify an error in a testfile. A exception
 * of type TestfileException will be thrown if the testfile is invalid.
 */
class TestfileException : public std::exception {
public:

	/**
	 * Constructor
	 * @param msg message of the exception
	 */
	TestfileException(std::string msg);


	/**
	 * Destructor
	 */
	~TestfileException();


	/**
	 * This function returns the message of the exception as a pointer.
	 * @return pointer of the message
	 */
	virtual const char* what() const throw();

private:
    std::string _M_msg;

};

#endif /* TESTFILEEXCEPTION_H_ */
