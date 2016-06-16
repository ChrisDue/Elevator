/*
 * ElevatorLogic.cpp
 *
 *  Created on: 20.06.2014
 *      Author: STSJR
 */

#include "ElevatorLogic.h"

#include <iostream>

#include "Interface.h"
#include "Person.h"
#include "Floor.h"
#include "Elevator.h"
#include "Event.h"
#include "Environment.h"

ElevatorLogic::ElevatorLogic() : EventHandler("ElevatorLogic"), moved_(false) {
}

ElevatorLogic::~ElevatorLogic() {
}

void ElevatorLogic::Initialize(Environment &env) {
	env.RegisterEventHandler("Interface::Notify", this, &ElevatorLogic::HandleNotify);
	env.RegisterEventHandler("Elevator::Stopped", this, &ElevatorLogic::HandleStopped);
	env.RegisterEventHandler("Elevator::Opened", this, &ElevatorLogic::HandleOpened);
	env.RegisterEventHandler("Elevator::Closed", this, &ElevatorLogic::HandleClosed);
}

void ElevatorLogic::HandleNotify(Environment &env, const Event &e) {

	Interface *interf = static_cast<Interface*>(e.GetSender());
	Loadable *loadable = interf->GetLoadable(0);

	if (loadable->GetType() == "Elevator") {

		Elevator *ele = static_cast<Elevator*>(loadable);

		env.SendEvent("Elevator::Open", 0, this, ele);
	}
}

void ElevatorLogic::HandleStopped(Environment &env, const Event &e) {

	Elevator *ele = static_cast<Elevator*>(e.GetSender());

	env.SendEvent("Elevator::Open", 0, this, ele);
}

void ElevatorLogic::HandleOpened(Environment &env, const Event &e) {

	Elevator *ele = static_cast<Elevator*>(e.GetSender());

	env.SendEvent("Elevator::Close", 4, this, ele);
}

void ElevatorLogic::HandleClosed(Environment &env, const Event &e) {

	Elevator *ele = static_cast<Elevator*>(e.GetSender());

	if (!moved_) {
		env.SendEvent("Elevator::Up", 0, this, ele);
		env.SendEvent("Elevator::Stop", 4, this, ele);

		moved_ = true;
	}
}
