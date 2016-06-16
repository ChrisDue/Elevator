/*
 * ElevatorEnvironment.cpp
 *
 *  Created on: 17.06.2014
 *      Author: STSJR
 */

#include "ElevatorEnvironment.h"

#include <set>
#include <iostream>

#include "Elevator.h"
#include "Floor.h"
#include "Interface.h"
#include "UpDownButton.h"
#include "Person.h"

ElevatorEnvironment::ElevatorEnvironment() {

	UseEntity<Interface>();
	UseEntity<UpDownButton>();
	UseEntity<Elevator>(&ElevatorEnvironment::NewElevator);
	UseEntity<Person>(&ElevatorEnvironment::NewPerson);
	UseLoadable<Floor>(&ElevatorEnvironment::NewFloor);

	RegisterEventHandler(&logic_);
}

ElevatorEnvironment::~ElevatorEnvironment() {
}

void ElevatorEnvironment::Evaluate() {

	for (Person *p : persons_) {
		if (p->GetCurrentFloor() != p->GetFinalFloor()) {
			throw LogicException("A person did not reach its desired destination floor.");
		}
	}
}

void ElevatorEnvironment::Validate() {

	if (elevators_.size() == 0)
		throw TestfileException("No elevator object was specified.");

	if (persons_.size() == 0)
		throw TestfileException("No person object was specified.");

	Floor *top = 0;
	Floor *bottom = 0;

	for (Floor *floor : floors_) {

		if (!floor->GetAbove()) {
			if (!top)
				top = floor;
			else
				throw TestfileException("Invalid floor configuration: multiple top floors exist.");
		}
		if (!floor->GetBelow()) {
			if (!bottom)
				bottom = floor;
			else
				throw TestfileException("Invalid floor configuration: multiple bottom floors exist.");
		}
	}

	if (!top)
		throw TestfileException("Invalid floor configuration: floor arrangement is cyclic.");

	unsigned int c = 0;
	while (top) {
		++c;
		top = top->GetBelow();
	}

	if (c != floors_.size())
		throw TestfileException("Invalid floor configuration: an additional cyclic floor arrangement exists.");

	for (Person *person : persons_) {

		std::set<Floor*> floors;
		std::set<Elevator*> elevators;

		floors.insert(person->GetCurrentFloor());

		bool added;
		do {
			added = false;

			for (Floor *floor : floors) {
				for (int i = 0; i < floor->GetInterfaceCount(); ++i) {

					Interface *interf = floor->GetInterface(i);

					for (int j = 0; j < interf->GetLoadableCount(); ++j) {

						Elevator *ele = static_cast<Elevator*>(interf->GetLoadable(j));

						added |= elevators.insert(ele).second;
					}
				}
			}

			for (Elevator *ele : elevators) {
				for (int i = 0; i < ele->GetInterfaceCount(); ++i) {

					Interface *interf = ele->GetInterface(i);

					for (int j = 0; j < interf->GetLoadableCount(); ++j) {

						Floor *floor = static_cast<Floor*>(interf->GetLoadable(j));

						added |= floors.insert(floor).second;
					}
				}
			}

		} while (added);

		if (!floors.count(person->GetFinalFloor())) {
			std::ostringstream oss("", std::ostringstream::ate);
			oss << person->GetName() << " has unreachable final floor: Floor " << person->GetFinalFloor()->GetId() << ".";
			throw TestfileException(oss.str());
		}
	}


	ValidateElevatorCanCarryAllPersons();
}

void ElevatorEnvironment::NewElevator(Elevator *elevator) {
	elevators_.push_back(elevator);
}

void ElevatorEnvironment::NewFloor(Floor *floor) {
	floors_.push_back(floor);
}

void ElevatorEnvironment::NewPerson(Person *person) {
	persons_.push_back(person);
}


/**
 * Validates the max loads of each elevator and the weight of each person.
 * Each elevator must be able to carry each person. So the max load of each elevator
 * must be bigger or equal than the biggest weight of all persons.
 */
void ElevatorEnvironment::ValidateElevatorCanCarryAllPersons() {

	int maxWeight_ = 0;
	int id_ = 0;
	for (Person *person : persons_) {
		if (person->GetWeight() > maxWeight_) {
			maxWeight_ = person->GetWeight();
			id_ = person->GetId();
		}
	}

	for (Elevator *ele : elevators_) {
		if (ele->GetMaxLoad() < maxWeight_) {

			std::ostringstream oss("", std::ostringstream::ate);
			oss << "The elevator " << ele->GetId() << " cannot carry the person " << id_ << " because of its weight.";
			throw TestfileException(oss.str());
		}
	}
}

