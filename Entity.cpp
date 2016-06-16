/*
 * Entity.cpp
 *
 *  Created on: 21.06.2014
 *      Author: STSJR
 */

#include <sstream>

#include "Entity.h"

Entity::Entity(const std::string &name) : Loadable(name), EventHandler(name) {
}

Entity::~Entity() {
}

void Entity::Load(Environment &env, std::istream &is) {
	Loadable::Load(env, is);

	std::ostringstream oss;
	oss << name_ << " " << GetId();
	name_ = oss.str();
}
