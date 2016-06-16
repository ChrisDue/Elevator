/*
 * DescriptionReader.h
 *
 *  Created on: 28.10.2015
 *      Author: CAT
 */
#ifndef DESCRIPTIONREADER_H_
#define DESCRIPTIONREADER_H_

#include <istream>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <limits>

#include "TestfileException.h"

/**
 * This class is used to read a description of a test file.
 */
class DescriptionReader {

public:

	/**
	 * Opens the file,reads the description of the test file and saves the description to the destination string.
	 * Multiple descriptions will be append of a new line.
	 * @param file reference of the file location
	 * @param dest pointer to the destination string
	 */
	static void Read(const std::string &file, std::string *dest);


	/**
	 * Ignores the description of the istream.
	 * @param is reference of istream
	 */
	static void IgnoreParameters( std::istream &is );

private:
	/**
	 * A helper function to find the description.
	 * @param is reference of istream
	 * @param dest pointer to the destination string
	 */
	static void ReadHelp( std::istream &stream, std::string *dest);


	/**
	 * Loads the parameter of a description.
	 * @param is reference of istream
	 * @param dest pointer to the destination string
	 */
	static void LoadParameter( std::string *dest, std::istream &is );


	/**
	 * Writes the given string in the ostringstream. If the given boolean is false, then appends a space in front of the string.
	 * @param firstString reference determines if a space is appended
	 * @param ret reference of the ostringstream
	 * @param s constant reference of the string which will be written in the ostringstream
	 */
	static void Writer(bool &firstString, std::ostringstream &ret, const std::string &s);
};


#endif
