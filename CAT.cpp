//============================================================================
// Name        : CAT.cpp
// Author      : Jonas Rinast
// Version     :
// Copyright   : Copyright by Jonas Rinast, Institute for Software Systems, Hamburg University of Technology
// Description : Compilation and Testing (CAT) C++ Framework
//============================================================================

#include <iostream>
#include <vector>

#include "ElevatorEnvironment.h"
#include "DescriptionReader.h"
#include "TestfileException.h"
#include "LogicException.h"

int main(int argc, char **argv) {

	if (argc < 2) {
		std::cout << "Usage: " + std::string(argv[0]) + " <testfile>" << std::endl;
		return 2;
	}

	std::string config = argv[1];

	try {

		ElevatorEnvironment env;

		std::cout << "Running test: " << config << std::endl;

		env.Load(config);
		env.Run();
		env.Evaluate();

	} catch(TestfileException &e)
	{
		std::cerr << "[TestfileException] " << e.what() << std::endl;
		return 2;

	} catch(LogicException &e) {

		std::cerr << "[LogicException] " << e.what() << std::endl;

		try {
			// reads the description
			std::string description_ = "";
			DescriptionReader::Read(config, &description_);

			// outputs the description if it is not empty
			if (description_ != "") {
				std::cerr << "Description of the testfile:" << std::endl << description_ << std::endl;
			}

		} catch (std::exception &e2) {

			std::cerr << "Cannot read description of the testfile." << std::endl;

		}
		return 3;


	} catch (std::exception &e) {

		std::cerr << "[Exception] " << e.what() << std::endl;


		try {
			// reads the description
			std::string description_ = "";
			DescriptionReader::Read(config, &description_);

			// outputs the description if it is not empty
			if (description_ != "") {
				std::cerr << "Description of the testfile:" << std::endl << description_ << std::endl;
			}

		} catch (std::exception &e2) {

			std::cerr << "Cannot read description of the testfile." << std::endl;

		}

		return 1;
	}

	std::cout << "Test " << config << " ran successfully." << std::endl;

	return 0;
}
