/*
 * Interface.h
 *
 *  Created on: 18.06.2014
 *      Author: STSJR
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include "Entity.h"

#include <string>

class Floor;
class Elevator;

class Interface: public Entity {

public:

	Interface();
	Interface(const std::string &name);
	virtual ~Interface();

	int GetLoadableCount() const;
	Loadable *GetLoadable(int n) const;
	bool HasLoadable(Loadable *e) const;

	void DeclareEvents(Environment &env);
	void Initialize(Environment &env);

	void Load(Environment &env, std::istream &is);
	void Validate();

private:

	void HandleInteract(Environment &env, const Event &e);

	int count_;
	Loadable **loadables_;
};

#endif /* INTERFACE_H_ */
