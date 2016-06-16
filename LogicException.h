/*
 * TestfileException.h
 *
 *  Created on: 02.12.2015
 *      Author: CAT
 */

#ifndef LOGICEXCEPTION_H_
#define LOGICEXCEPTION_H_

#include <exception>
#include <string>

/**
 * This class is used as an exception. An object of this class will be thrown if a test run fails
 * because of the elevator logic.
 */
class LogicException : public std::exception {
public:

	/**
	 * Constructor
	 * @param msg message of the exception
	 */
	LogicException(std::string msg);


	/**
	 * Destructor
	 */
	~LogicException();


	/**
	 * This function returns the message of the exception as a pointer.
	 * @return pointer of the message
	 */
	virtual const char* what() const throw();

private:
    std::string _M_msg;

};

#endif /* LOGICEXCEPTION_H_ */
