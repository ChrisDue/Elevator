/*
 * Entity.h
 *
 *  Created on: 17.06.2014
 *      Author: STSJR
 */

#ifndef ENTITY_H_
#define ENTITY_H_

#include "Loadable.h"
#include "EventHandler.h"

class Entity : public Loadable, public EventHandler {

public:

	Entity(const std::string &name);
	virtual ~Entity();

	virtual void Load(Environment &env, std::istream &is);
};

#endif /* ENTITY_H_ */
