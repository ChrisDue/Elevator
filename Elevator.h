/*
 * Elevator.h
 *
 *  Created on: 18.06.2014
 *      Author: STSJR
 */

#ifndef ELEVATOR_H_
#define ELEVATOR_H_

#include "Entity.h"
#include <math.h>


class Floor;
class Interface;

class Elevator: public Entity {
public:

	enum Movement {
		Idle,
		Up,
		Down,
		Malfunction
	};

	Elevator();
	virtual ~Elevator();

	bool HasFloor(Floor *floor) const;
	bool IsLowestFloor(Floor *floor) const;
	bool IsHighestFloor(Floor *floor) const;
	bool IsOpen() const;

	int GetSpeed() const;
	int GetMaxLoad() const;
	Floor *GetCurrentFloor() const;

	int GetInterfaceCount() const;
	Interface *GetInterface(int n) const;

	double GetPosition() const;
	Movement GetState() const;


	void DeclareEvents(Environment &env);
	void Initialize(Environment &env);

	void Load(Environment &env, std::istream &is);
	void Validate();

private:

	void HandleUpdate(Environment &env, const Event &e);
	void HandleUp(Environment &env, const Event &e);
	void HandleDown(Environment &env, const Event &e);
	void HandleStop(Environment &env, const Event &e);
	void HandleOpen(Environment &env, const Event &e);
	void HandleClose(Environment &env, const Event &e);
	void HandleBeep(Environment &env, const Event &e);
	void HandleBeeping(Environment &env, const Event &e);
	void HandleStopBeep(Environment &env, const Event &e);
	void HandleMalfunction(Environment &env, const Event &e);
	void HandleFixed(Environment &env, const Event &e);
	void HandleOpened(Environment &env, const Event &e);
	void HandleClosed(Environment &env, const Event &e);

	/**
	 * Checks if the middle of each floor is reachable from the elevator.
	 * @param lowestFloorOfElevator pointer of the lowest floor which has an interface to the elevator
	 * @param highestFloorOfElevator pointer of the highest floor which has an interface to the elevator
	 * @return pointer of the floor which is unreachable, otherwise returns null pointer
	 */
	Floor* CanReachFloor(Floor *lowestFloorOfElevator, Floor *highestFloorOfElevator);

	int speed_;
	int load_;
	Floor *current_;
	int count_;
	Interface** interfs_;

	double position_;
	int lasttime_;
	Movement state_;
	int beeping_;
	bool isOpen_;
};

#endif /* ELEVATOR_H_ */
