/*
 * Person.cpp
 *
 *  Created on: 18.06.2014
 *      Author: STSJR
 */

#include "Person.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "Floor.h"
#include "Interface.h"
#include "Elevator.h"
#include "Event.h"
#include "Environment.h"

Person::Person() :
		Entity("Person"), current_(0), final_(0), giveup_(0), weight_(0), start_(0), isEnteringOrExiting_(false), requested_(nullptr), elevator_(nullptr), beeping_(false), action_(0) {
}

Person::~Person() {
}

Floor *Person::GetCurrentFloor() const {
	return current_;
}

Floor *Person::GetFinalFloor() const {
	return final_;
}

int Person::GetGiveUpTime() const {
	return giveup_;
}

int Person::GetWeight() const {
	return weight_;
}

Elevator *Person::GetCurrentElevator() const {
	return elevator_;
}

void Person::DeclareEvents(Environment &env) {
	env.DeclareEvent("Person::Entering");
	env.DeclareEvent("Person::Entered");
	env.DeclareEvent("Person::Exiting");
	env.DeclareEvent("Person::Exited");
	env.DeclareEvent("Person::Canceled");
}

void Person::Initialize(Environment &env) {
	env.RegisterEventHandler("UpDownButton::Decide", this, &Person::HandleDecide);
	env.RegisterEventHandler("Elevator::Opened", this, &Person::HandleOpened);
	env.RegisterEventHandler("Elevator::Beeping", this, &Person::HandleBeeping);
	env.RegisterEventHandler("Elevator::Beeped", this, &Person::HandleBeeped);
	env.RegisterEventHandler("Person::Entered", this, &Person::HandleEntered);
	env.RegisterEventHandler("Person::Exiting", this, &Person::HandleExiting);
	env.RegisterEventHandler("Person::Exited", this, &Person::HandleExited);
	env.RegisterEventHandler("Elevator::Closed", this, &Person::HandleClosed);
	env.RegisterEventHandler("Elevator::Moving", this, &Person::HandleMoving);
	env.RegisterEventHandler("Elevator::Fixed", this, &Person::HandleFixed);
	env.RegisterEventHandler("Elevator::Malfunction", this, &Person::HandleMalfunction);
	env.RegisterEventHandler("Interface::Notify", this, &Person::HandleNotify);
	env.RegisterEventHandler("Elevator::Closing", this, &Person::HandleClosing);

	std::list<Floor*> init;
	init.push_front(current_);

	std::list<std::list<Floor*>> paths;
	paths.push_front(init);

	do {

		std::list<Floor*> path = paths.front();
		Floor *current = path.back();

		paths.pop_front();

		if (current == final_) {
			path.pop_front();
			path_ = path;
			break;
		}

		for (int i = 0; i < current->GetInterfaceCount(); ++i) {

			Interface *interf = current->GetInterface(i);
			Elevator *ele = static_cast<Elevator*>(interf->GetLoadable(0));

			for (int j = 0; j < ele->GetInterfaceCount(); ++j) {

				Interface *binterf = ele->GetInterface(j);
				Floor *floor = static_cast<Floor*>(binterf->GetLoadable(0));

				if (floor != current) {
					path.push_back(floor);
					paths.push_back(path);
					path.pop_back();
				}
			}
		}

	} while (true);

	std::cout << "Path for " << GetName() << " is ";
	for (Floor *floor : path_) {
		std::cout << "Floor " << floor->GetId() << " ";
	}
	std::cout << std::endl;

	RequestElevator(env, start_);
}

void Person::Load(Environment &env, std::istream &is) {

	Entity::Load(env, is);

	env.LoadReference(&current_, is);
	env.LoadReference(&final_, is);
	env.LoadInteger(&giveup_, is);
	env.LoadInteger(&weight_, is);
	env.LoadInteger(&start_, is);
}

void Person::Validate() {

	Entity::Validate();

	if (!current_) {
		std::ostringstream oss("Starting floor of ", std::ostringstream::ate);
		oss << GetName() << " is null.";
		throw TestfileException(oss.str());
	}

	if (current_->GetType() != "Floor") {
		std::ostringstream oss("Starting floor of ", std::ostringstream::ate);
		oss << GetName() << " does not reference a floor object.";
		throw TestfileException(oss.str());
	}

	if (!final_) {
		std::ostringstream oss("Final floor of ", std::ostringstream::ate);
		oss << GetName() << " is null.";
		throw TestfileException(oss.str());
	}

	if (final_->GetType() != "Floor") {
		std::ostringstream oss("Final floor of ", std::ostringstream::ate);
		oss << GetName() << " does not reference a floor object.";
		throw TestfileException(oss.str());
	}

	if (giveup_ <= 0) {
		std::ostringstream oss("Give up time of ", std::ostringstream::ate);
		oss << GetName() << " must be positive.";
		throw TestfileException(oss.str());
	}

	if (weight_ <= 0) {
		std::ostringstream oss("Weight of ", std::ostringstream::ate);
		oss << GetName() << " must be positive.";
		throw TestfileException(oss.str());
	}

	if (start_ < 0) {
		std::ostringstream oss("Start time of ", std::ostringstream::ate);
		oss << GetName() << " must be non-negative.";
		throw TestfileException(oss.str());
	}
}


void Person::HandleDecide(Environment &env, const Event &e) {

	if (ForMe(e)) {
		if (current_->IsAbove(path_.front()))
			env.SendEvent("UpDownButton::Up", 0, this, e.GetSender());

		if (current_->IsBelow(path_.front()))
			env.SendEvent("UpDownButton::Down", 0, this, e.GetSender());
	}
}

void Person::HandleOpened(Environment &env, const Event &e) {

	Elevator *ele = static_cast<Elevator*>(e.GetSender());

	if (elevator_) {

		if (elevator_ == ele)
			if (ele->GetPosition() > 0.49 && ele->GetPosition() < 0.51)
				if (ele->GetCurrentFloor() == path_.front())
					Exit(env);

	} else if (requested_->HasLoadable(ele) && ele->GetCurrentFloor() == current_ && ele->GetPosition() > 0.49
			&& ele->GetPosition() < 0.51 && e.GetTime() > start_ && ele->GetState() != Elevator::Malfunction && current_ != final_) {

		elevator_ = ele;
		Enter(env);
	}
}

void Person::HandleEntered(Environment &env, const Event &e) {

	if (FromMe(e) ) {
		isEnteringOrExiting_ = false;

		if (elevator_->GetState() != Elevator::Malfunction) {
			RequestFloor(env);
		} else {
			Exit(env);
		}
	}
}

void Person::HandleExiting(Environment &env, const Event &e) {

	Person *person = static_cast<Person*>(e.GetSender());
	Elevator *ele = person->GetCurrentElevator();

	if (beeping_ && elevator_ == ele && !FromMe(e))
		exiting_.insert(person);
}

void Person::HandleExited(Environment &env, const Event &e) {

	Person *person = static_cast<Person*>(e.GetSender());

	if (FromMe(e)) {
		exiting_.clear();

		isEnteringOrExiting_ = false;

		if (elevator_) {
			current_ = elevator_->GetCurrentFloor();
			elevator_ = nullptr;
		}
		if (!beeping_) {

			if (current_ == path_.front())
				path_.pop_front();

			RequestElevator(env, e.GetTime() + 3);
		}
	} else if (beeping_ && exiting_.count(person)) {
		exiting_.erase(person);
		Cancel(env);
		Exit(env);
	}
}

void Person::HandleBeeping(Environment &env, const Event &e) {

	Elevator *ele = static_cast<Elevator*>(e.GetSender());

	if (elevator_ == ele && elevator_) {

		beeping_ = true;
		env.CancelEvent(action_); // Cancel Interface::Interact
		Exit(env);
	}
}

void Person::HandleBeeped(Environment &env, const Event &e) {

	Elevator *ele = static_cast<Elevator*>(e.GetSender());

	if (beeping_ && elevator_ == ele) {

		beeping_ = false;
		Cancel(env);
		RequestFloor(env);
	}
}

void Person::HandleMoving(Environment &env, const Event &e) {

	if (beeping_) {

		beeping_ = false;
		// RequestElevator(env, e.GetTime() + 3);
	}
}

void Person::HandleFixed(Environment &env, const Event &e) {

	Elevator *ele = static_cast<Elevator*>(e.GetSender());

	if (ele == elevator_) {
		RequestFloor(env);
	}
}

void Person::HandleMalfunction(Environment &env, const Event &e) {

	Elevator *ele = static_cast<Elevator*>(e.GetSender());

	if (ele == elevator_ && ele->IsOpen()) {
		Cancel(env);
		Exit(env);
	}
}

void Person::HandleClosed(Environment &env, const Event &e) {

	Elevator *ele = static_cast<Elevator*>(e.GetSender());

	if (e.GetTime() > start_ && ele->GetCurrentFloor() == current_ && !elevator_ && requested_->HasLoadable(ele)) {

		RequestElevator(env, e.GetTime() + 3);

	}
}

void Person::HandleNotify(Environment &env, const Event &e) {

	if (ForMe(e) && !elevator_) {




		for (int i=0; i<requested_->GetLoadableCount(); i++) {

			Elevator *ele = static_cast<Elevator*>(requested_->GetLoadable(i));

			if (ele->IsOpen() && ele->GetCurrentFloor() == current_ && ele->GetState() != Elevator::Malfunction) {

				elevator_ = ele;
				Enter(env);
				break;

			}
		}
	}
}

void Person::HandleClosing(Environment &env, const Event &e) {

	Elevator *ele = static_cast<Elevator*>(e.GetSender());

	if (elevator_ == ele && isEnteringOrExiting_) {

		Cancel(env);

		elevator_ = nullptr;

		isEnteringOrExiting_ = false;

		RequestElevator(env, e.GetTime() + 3);

	}
}


void Person::Enter(Environment &env) {

	if (current_ == final_) {

		// The person is at the destination floor
		elevator_ = nullptr;

	} else {


		// The person is not at the destination floor
		env.SendEvent("Person::Entering", 0, this, elevator_);
		action_ = env.SendEvent("Person::Entered", 3, this, elevator_);
		isEnteringOrExiting_ = true;

	}

}

void Person::Exit(Environment &env) {

	env.SendEvent("Person::Exiting", 0, this, elevator_);
	action_ = env.SendEvent("Person::Exited", 3, this, elevator_);
	isEnteringOrExiting_ = true;
}

void Person::Cancel(Environment &env) {

	if (action_) {

		env.CancelEvent(action_);
		env.SendEvent("Person::Canceled", 0, this);

		action_ = 0;

		isEnteringOrExiting_ = false;
	}
}

void Person::RequestFloor(Environment &env) {

	Floor *next = path_.front();

	if (!next) {
		std::ostringstream oss("Invalid operation: no next floor in path ", std::ostringstream::ate);
		oss << "ID of person: <" << GetId();
		oss << ", this error should never occur.";
		throw std::runtime_error(oss.str());
	}

	for (int i = 0; i < elevator_->GetInterfaceCount(); ++i) {

		Interface *interf = elevator_->GetInterface(i);
		Floor *floor = static_cast<Floor*>(interf->GetLoadable(0));

		if (floor == next) {
			action_ = env.SendEvent("Interface::Interact", 3, this, elevator_->GetInterface(i));

			return;
		}
	}

	std::ostringstream oss("Invalid operation: ", std::ostringstream::ate);
	oss << elevator_->GetName() << " can not go to Floor " << next->GetId();
	oss << ", this error should never occur.";
	throw std::runtime_error(oss.str());
}

void Person::RequestElevator(Environment &env, int time) {

	if (current_ == final_)
		return;

	Floor *next = path_.front();

	if (!next) {
		std::ostringstream oss("Invalid operation: no next floor in path ", std::ostringstream::ate);
		oss << "ID of person: <" << GetId();
		oss << ", this error should never occur.";
		throw std::runtime_error(oss.str());
	}

	for (int i = 0; i < current_->GetInterfaceCount(); ++i) {

		Interface *interf = current_->GetInterface(i);
		Elevator *ele = static_cast<Elevator*>(interf->GetLoadable(0));

		if (ele->HasFloor(next)) {

			requested_ = current_->GetInterface(i);
			env.SendEvent("Interface::Interact", time - env.GetClock(), this, requested_);

			return;
		}
	}

	std::ostringstream oss("Invalid operation: no elevator available to go from Floor ", std::ostringstream::ate);
	oss << current_->GetId() << " to Floor " << next->GetId();
	oss << ", this error should never occur.";
	throw std::runtime_error(oss.str());
}

