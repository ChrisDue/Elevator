/*
 * TestfileException.cpp
 *
 *  Created on: 26.11.2015
 *      Author: CAT
 */
#include "TestfileException.h"

/**
 * Constructor
 * @param msg message of the exception
 */
TestfileException::TestfileException(std::string value) :
		_M_msg(value) {

}

/**
 * Destructor
 */
TestfileException::~TestfileException() {

}

/**
 * This function returns the message of the exception as a pointer.
 * @return pointer of the message
 */
const char* TestfileException::what() const throw () {
	return _M_msg.c_str();
}

