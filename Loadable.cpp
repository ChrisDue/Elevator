/*
 * Loadable.cpp
 *
 *  Created on: 21.06.2014
 *      Author: STSJR
 */

#include "Loadable.h"

#include <sstream>
#include <stdexcept>

#include "Environment.h"

Loadable::Loadable(const std::string &name) : type_(name), id_(0) {
}

Loadable::~Loadable() {
}

int Loadable::GetId() const {
	return id_;
}

const std::string &Loadable::GetType() const {
	return type_;
}

void Loadable::Load(Environment &env, std::istream &is) {
	env.LoadInteger(&id_, is);
}

void Loadable::Validate() {

	if (id_ <= 0)
		throw TestfileException("Reference ids must be positive.");
}
