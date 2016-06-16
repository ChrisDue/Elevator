/*
 * Floor.h
 *
 *  Created on: 18.06.2014
 *      Author: STSJR
 */

#ifndef FLOOR_H_
#define FLOOR_H_

#include "Entity.h"

class Interface;
class Elevator;

class Floor: public Loadable {
public:
	Floor();
	virtual ~Floor();

	bool HasElevator(Elevator *ele) const;
	bool IsAbove(Floor *floor) const;
	bool IsBelow(Floor *floor) const;

	Floor *GetBelow() const;
	Floor *GetAbove() const;
	int GetHeight() const;

	int GetInterfaceCount() const;
	Interface *GetInterface(int n) const;

	void Load(Environment &env, std::istream &is);
	void Validate();

private:
	Floor *below_;
	Floor *above_;
	int height_;
	int count_;
	Interface **interfs_;
};

#endif /* FLOOR_H_ */
