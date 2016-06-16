/*
 * TestfileException.cpp
 *
 *  Created on: 02.12.2015
 *      Author: CAT
 */
#include "LogicException.h"

/**
 * Constructor
 * @param msg message of the exception
 */
LogicException::LogicException(std::string value) :
		_M_msg(value) {

}

/**
 * Destructor
 */
LogicException::~LogicException() {

}

/**
 * This function returns the message of the exception as a pointer.
 * @return pointer of the message
 */
const char* LogicException::what() const throw () {
	return _M_msg.c_str();
}

