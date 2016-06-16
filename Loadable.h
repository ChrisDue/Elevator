/*
 * Loadable.h
 *
 *  Created on: 17.06.2014
 *      Author: STSJR
 */

#ifndef LOADABLE_H_
#define LOADABLE_H_

#include <iosfwd>
#include <string>

#include "TestfileException.h"

class Environment;

class Loadable {

public:
	Loadable(const std::string &name);
	virtual ~Loadable();

	int GetId() const;
	const std::string &GetType() const;

	virtual void Load(Environment &env, std::istream &is);
	virtual void Validate();

private:
	std::string type_;
	int id_;
};

#endif /* LOADABLE_H_ */
