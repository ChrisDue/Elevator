/*
 * Person.h
 *
 *  Created on: 18.06.2014
 *      Author: STSJR
 */

#ifndef PERSON_H_
#define PERSON_H_

#include "Entity.h"

#include <set>
#include <list>

#include "TestfileException.h"

class Floor;
class Interface;
class Elevator;

class Person: public Entity {

public:

	Person();
	virtual ~Person();

	Floor *GetCurrentFloor() const;
	Floor *GetFinalFloor() const;
	int GetGiveUpTime() const;
	int GetWeight() const;
	Elevator *GetCurrentElevator() const;

	void DeclareEvents(Environment &env);
	void Initialize(Environment &env);

	void Load(Environment &env, std::istream &is);
	void Validate();

private:

	void HandleDecide(Environment &env, const Event &e);
	void HandleOpened(Environment &env, const Event &e);
	void HandleEntered(Environment &env, const Event &e);
	void HandleExiting(Environment &env, const Event &e);
	void HandleExited(Environment &env, const Event &e);
	void HandleBeeping(Environment &env, const Event &e);
	void HandleBeeped(Environment &env, const Event &e);
	void HandleMoving(Environment &env, const Event &e);
	void HandleFixed(Environment &env, const Event &e);
	void HandleMalfunction(Environment &env, const Event &e);
	void HandleClosed(Environment &env, const Event &e);
	void HandleNotify(Environment &env, const Event &e);
	void HandleClosing(Environment &env, const Event &e);

	void Enter(Environment &env);
	void Exit(Environment &env);
	void Cancel(Environment &env);

	void RequestFloor(Environment &env);
	void RequestElevator(Environment &env, int time);

	Floor *current_;
	Floor *final_;
	int giveup_;
	int weight_;
	int start_;
	bool isEnteringOrExiting_;

	Interface *requested_;
	Elevator *elevator_;
	bool beeping_;
	int action_;
	std::list<Floor*> path_;
	std::set<Person*> exiting_;


};

#endif /* PERSON_H_ */
