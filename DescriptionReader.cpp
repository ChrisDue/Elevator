/*
 * DescriptionReader.cpp
 *
 *  Created on: 28.10.2014
 *      Author: CAT
 */

#include "DescriptionReader.h"

/**
* Opens the file,reads the description of the test file and saves the description to the destination string.
* Multiple descriptions will be append of a new line.
* @param file reference of the file location
* @param dest pointer to the destination string
*/
void DescriptionReader::Read(const std::string &file, std::string *dest) {


	// opens the file
	std::ifstream is(file);

	// checks if the file is open
	if (!is.is_open()) {
		throw std::runtime_error( "Error: could not open file: " + file + ".");
	}
	else {

		// reads the description
		DescriptionReader::ReadHelp(is, dest);

	}

	// closes the file
	is.close();
}


/**
 * Ignores the description of the istream.
 * @param is reference of istream
 */
void DescriptionReader::IgnoreParameters( std::istream &is ) {
	std::string str_ = "";
	LoadParameter( &str_, is );
}

/**
 * A helper function to find the description.
 * @param is reference of istream
 * @param dest pointer to the destination string
 */
void DescriptionReader::ReadHelp(std::istream& is, std::string *dest) {

	std::string s_ = "";
	bool isFirstDescription_ = true; // checks if it is the first description


	while (!is.eof()) {

		std::string stringValue;

		if (!is.eof())
			is >> stringValue;

		// ignores the corresponded line
		if (stringValue != "Description") {
			is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			continue;
		} else {

			// loads the parameters
			DescriptionReader::LoadParameter(&s_, is);

			// appends another description
			if (!isFirstDescription_){
				*dest += "\n\n";
			}
			*dest += s_;
			isFirstDescription_ = false;
		}
	}
}


/**
 * Loads the parameter of a description.
 * @param is reference of istream
 * @param dest pointer to the destination string
 */
void DescriptionReader::LoadParameter(std::string *dest, std::istream &is) {

	// invalid end of istream
	if (is.eof()) {
		throw TestfileException(
				"Invalid description: expected '{', but sequence is empty.");
	}

	std::ostringstream ret;
	std::string s;
	is >> s;

	// first character must be a bracket
	if (s.size() != 1 || *s.begin() != '{') {
		throw TestfileException("Invalid description: expected '{'.");
	}

	int o = 1; // counter for the brackets
	bool firstString = true;

	while (o > 0) {

		// if a break is reached, appends a break to the ostringstream
		if (is.peek() == '\n')
			ret << "\n";

		// invalid end of file
		if (is.eof())
			throw TestfileException("Invalid description: expected '}', but sequence is empty.");

		// clears the string and reads the next one
		s = "";
		is >> s;


		// if the next read string is a left parenthesis, then increases the counter for the brackets
		if (s.size() == 1 && *s.begin() == '{') {
			o++;

			if (o > 1) {
				DescriptionReader::Writer(firstString, ret, s);
			}

		}
		// if the next read string is a right parenthesis, then decreases the counter for the brackets
		else if (s.size() == 1 && *s.begin() == '}') {

			if (o > 1) {
				DescriptionReader::Writer(firstString, ret, s);
			}
			o--;
		}

		else {

			// writes the string in the ostringstream
			DescriptionReader::Writer(firstString, ret, s);
		}
	}

	// converts the ostringstream to the destination string
	*dest = ret.str();
}

/**
 * Writes the given string in the ostringstream. If the given boolean is false, then appends a space in front of the string.
 * @param firstString reference determines if a space is appended
 * @param ret reference of the ostringstream
 * @param s constant reference of the string which will be written in the ostringstream
 */
void DescriptionReader::Writer(bool &firstString, std::ostringstream &ret, const std::string &s) {
	if (!firstString)
		ret << ' ';
	ret << s;
	firstString = false;
}
