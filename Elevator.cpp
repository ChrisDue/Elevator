/*
 * Elevator.cpp
 *
 *  Created on: 18.06.2014
 *      Author: STSJR
 */

#include "Elevator.h"

#include <set>
#include <stdexcept>

#include "Environment.h"
#include "Floor.h"
#include "Interface.h"

Elevator::Elevator() :
		Entity("Elevator"), speed_(0), load_(0), current_(0), count_(0), interfs_(
				0), position_(0.5), lasttime_(0), state_(Idle), beeping_(0), isOpen_(false) {
}

Elevator::~Elevator() {
	if (count_ > 0) {
		delete[] interfs_;
	}
}

bool Elevator::HasFloor(Floor *floor) const {

	for (int i = 0; i < count_; ++i) {

		Interface *interf = interfs_[i];

		if (interf->GetLoadable(0) == floor)
			return true;
	}

	return false;
}

bool Elevator::IsLowestFloor(Floor *floor) const {

	for (int i = 0; i < count_; ++i) {

		Interface *interf = interfs_[i];
		Floor *candidate = static_cast<Floor*>(interf->GetLoadable(0));

		if (floor->IsBelow(candidate))
			return false;
	}

	return true;
}

bool Elevator::IsHighestFloor(Floor *floor) const {

	for (int i = 0; i < count_; ++i) {

		Interface *interf = interfs_[i];
		Floor *candidate = static_cast<Floor*>(interf->GetLoadable(0));

		if (floor->IsAbove(candidate))
			return false;
	}

	return true;
}

bool Elevator::IsOpen() const {
	return isOpen_;
}


int Elevator::GetSpeed() const {
	return speed_;
}

int Elevator::GetMaxLoad() const {
	return load_;
}

Floor *Elevator::GetCurrentFloor() const {
	return current_;
}

int Elevator::GetInterfaceCount() const {
	return count_;
}

Interface *Elevator::GetInterface(int n) const {
	return interfs_[n];
}

double Elevator::GetPosition() const {
	return position_;
}

Elevator::Movement Elevator::GetState() const {
	return state_;
}

void Elevator::DeclareEvents(Environment &env) {
	env.DeclareEvent("Elevator::Up");
	env.DeclareEvent("Elevator::Down");
	env.DeclareEvent("Elevator::Moving");
	env.DeclareEvent("Elevator::Stop");
	env.DeclareEvent("Elevator::Stopped");
	env.DeclareEvent("Elevator::Open");
	env.DeclareEvent("Elevator::Opening");
	env.DeclareEvent("Elevator::Opened");
	env.DeclareEvent("Elevator::Close");
	env.DeclareEvent("Elevator::Closing");
	env.DeclareEvent("Elevator::Closed");
	env.DeclareEvent("Elevator::Beep");
	env.DeclareEvent("Elevator::Beeping");
	env.DeclareEvent("Elevator::StopBeep");
	env.DeclareEvent("Elevator::Beeped");
	env.DeclareEvent("Elevator::Malfunction", true);
	env.DeclareEvent("Elevator::Fixed", true);
}

void Elevator::Initialize(Environment &env) {
	env.RegisterEventHandler("Environment::All", this, &Elevator::HandleUpdate);
	env.RegisterEventHandler("Elevator::Up", this, &Elevator::HandleUp);
	env.RegisterEventHandler("Elevator::Down", this, &Elevator::HandleDown);
	env.RegisterEventHandler("Elevator::Stop", this, &Elevator::HandleStop);
	env.RegisterEventHandler("Elevator::Open", this, &Elevator::HandleOpen);
	env.RegisterEventHandler("Elevator::Close", this, &Elevator::HandleClose);
	env.RegisterEventHandler("Elevator::Beep", this, &Elevator::HandleBeep);
	env.RegisterEventHandler("Elevator::Beeping", this, &Elevator::HandleBeeping);
	env.RegisterEventHandler("Elevator::StopBeep", this, &Elevator::HandleStopBeep);
	env.RegisterEventHandler("Elevator::Malfunction", this,
			&Elevator::HandleMalfunction);
	env.RegisterEventHandler("Elevator::Fixed", this, &Elevator::HandleFixed);

	env.RegisterEventHandler("Elevator::Opened", this, &Elevator::HandleOpened);
	env.RegisterEventHandler("Elevator::Closed", this, &Elevator::HandleClosed);
}

void Elevator::Load(Environment &env, std::istream &is) {

	Entity::Load(env, is);

	env.LoadInteger(&speed_, is);
	env.LoadInteger(&load_, is);
	env.LoadReference(&current_, is);
	env.LoadInteger(&count_, is);

	interfs_ = new Interface*[count_];

	for (int i = 0; i < count_; ++i) {
		env.LoadReference(&interfs_[i], is);
	}
}

void Elevator::Validate() {

	Entity::Validate();

	if (speed_ <= 0) {
		std::ostringstream oss("Speed of ", std::ostringstream::ate);
		oss << GetName() << " must be positive.";
		throw TestfileException(oss.str());
	}

	if (load_ < 10) {
		std::ostringstream oss("Maximum load of ", std::ostringstream::ate);
		oss << GetName() << " must be greater or equal to 10.";
		throw TestfileException(oss.str());
	}

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

	if (count_ < 2) {
		std::ostringstream oss("", std::ostringstream::ate);
		oss << GetName() << " must have at least 2 interfaces.";
		throw TestfileException(oss.str());
	}

	std::set<Interface*> unique;

	Floor *lowest = nullptr;
	Floor *highest = nullptr;

	for (int i = 0; i < count_; ++i) {

		Interface *interf = interfs_[i];

		if (!interf) {
			std::ostringstream oss("An interface of ", std::ostringstream::ate);
			oss << GetName() << " is null.";
			throw TestfileException(oss.str());
		}

		if (unique.count(interf)) {
			std::ostringstream oss("Interfaces used by ", std::ostringstream::ate);
			oss << GetName() << " are not unique.";
			throw TestfileException(oss.str());
		}
		unique.insert(interf);

		if (interf->GetType() != "Interface") {
			std::ostringstream oss("Interface ", std::ostringstream::ate);
			oss << interf->GetId() << " of ";
			oss << GetName() << " is not an interface object.";
			throw TestfileException(oss.str());
		}

		if (interf->GetLoadableCount() != 1) {
			std::ostringstream oss("", std::ostringstream::ate);
			oss << interf->GetName() << " of ";
			oss << GetName() << " must reference exactly one object.";
			throw TestfileException(oss.str());
		}

		Loadable *loadable = interf->GetLoadable(0);

		if (!loadable) {
			std::ostringstream oss("Referenced object of ", std::ostringstream::ate);
			oss << interf->GetName() << " of ";
			oss << GetName() << " is null.";
			throw TestfileException(oss.str());
		}

		if (loadable->GetType() != "Floor") {
			std::ostringstream oss("Referenced object of ", std::ostringstream::ate);
			oss << interf->GetName() << " of ";
			oss << GetName() << " must be an floor object.";
			throw TestfileException(oss.str());
		}

		Floor *floor = static_cast<Floor*>(loadable);

		if (!floor->HasElevator(this)) {
			std::ostringstream oss("Floor ", std::ostringstream::ate);
			oss << floor->GetId() << " referenced by ";
			oss << interf->GetName() << " of ";
			oss << GetName() << " does not point back to the elevator.";
			throw TestfileException(oss.str());
		}

		if (!lowest)
			lowest = floor;
		else if (lowest->IsBelow(floor)) {
			lowest = floor;
		}

		if (!highest)
			highest = floor;
		else if (highest->IsAbove(floor)) {
			highest = floor;
		}
	}

	if (highest->IsAbove(current_) || lowest->IsBelow(current_)) {
		std::ostringstream oss("Starting floor of ", std::ostringstream::ate);
		oss << GetName() << " is not between Floor " << lowest->GetId() << " and Floor " << highest->GetId() << ".";
		throw TestfileException(oss.str());
	}

	// validate if the elevator can start in the starting floor
	if (!HasFloor(this->current_)) {
		std::ostringstream oss("Elevator ", std::ostringstream::ate);
		oss << GetId() << " can not start in Floor " << current_->GetId() << " because the Floor has no interface to this Elevator.";
		throw TestfileException(oss.str());
	}

	Floor* floorTmp = CanReachFloor(lowest, highest);
	if (floorTmp) {
		std::ostringstream oss("Elevator ", std::ostringstream::ate);
		oss << GetId() << " can not reach center of Floor " << floorTmp->GetId() << ".";
		throw TestfileException(oss.str());
	}
}


/**
 * Checks if the middle of each floor is reachable from the elevator.
 * @param lowestFloorOfElevator pointer of the lowest floor which has an interface to the elevator
 * @param highestFloorOfElevator pointer of the highest floor which has an interface to the elevator
 * @return pointer of the floor which is unreachable, otherwise returns null pointer
 */
Floor* Elevator::CanReachFloor(Floor *lowestFloorOfElevator, Floor *highestFloorOfElevator) {

	bool currentFloorIsHighestFloor = true;


	// pointer to the current floor, which will be checked
	Floor *currentFloor = lowestFloorOfElevator;

	// determines the half of the height of the current floor
	double halfHeightOfCurrentFloor = currentFloor->GetHeight() / 2.0;

	// height counter is used to check if an elevator can reach a floor
	double heightCounter = halfHeightOfCurrentFloor;

	// take the next floor above
	currentFloor = currentFloor->GetAbove();

	while (currentFloorIsHighestFloor) {

		// determines the half of the height of the current floor
		halfHeightOfCurrentFloor = currentFloor->GetHeight() / 2.0;

		if (!HasFloor(currentFloor)) {

			heightCounter += currentFloor->GetHeight();


		} else {

			heightCounter += halfHeightOfCurrentFloor;

			double value = fmod(heightCounter, speed_);
			heightCounter += halfHeightOfCurrentFloor;

			// checks the determined value if it is unequal zero
			if (value != 0) {
				return currentFloor;
			}
		}


		// checks if the current floor is the highest floor
		if (currentFloor == highestFloorOfElevator) {
			currentFloorIsHighestFloor = false;
		} else {
			currentFloor = currentFloor->GetAbove();
		}
	}

	return nullptr;
}


void Elevator::HandleUpdate(Environment &env, const Event &e) {

	double dist = speed_ * (e.GetTime() - lasttime_);

	if (state_ == Up) {

		double remaining = current_->GetHeight() * (1.0 - position_);

		while (dist >= remaining && current_->GetAbove()) {
			dist -= remaining;
			current_ = current_->GetAbove();
			remaining = current_->GetHeight();
			position_ = 0.0;
		}

		position_ += dist / current_->GetHeight();
	}
	if (state_ == Down) {

		double remaining = current_->GetHeight() * position_;

		while (dist >= remaining && current_->GetBelow()) {
			dist -= remaining;
			current_ = current_->GetBelow();
			remaining = current_->GetHeight();
			position_ = 1.0;
		}

		position_ -= dist / current_->GetHeight();
	}


	lasttime_ = e.GetTime();
}

void Elevator::HandleUp(Environment &env, const Event &e) {

	if (ForMe(e)) {
		state_ = Up;
		env.SendEvent("Elevator::Moving", 0, this);
	}
}

void Elevator::HandleDown(Environment &env, const Event &e) {

	if (ForMe(e)) {
		state_ = Down;
		env.SendEvent("Elevator::Moving", 0, this);
	}
}

void Elevator::HandleStop(Environment &env, const Event &e) {

	if (ForMe(e)) {
		state_ = Idle;
		env.SendEvent("Elevator::Stopped", 0, this);
	}
}

void Elevator::HandleOpen(Environment &env, const Event &e) {

	if (ForMe(e)) {
		env.SendEvent("Elevator::Opening", 0, this);
		env.SendEvent("Elevator::Opened", 3, this);
	}
}

void Elevator::HandleClose(Environment &env, const Event &e) {

	if (ForMe(e)) {
		isOpen_ = false;
		env.SendEvent("Elevator::Closing", 0, this);
		env.SendEvent("Elevator::Closed", 3, this);
		if (position_ > 0.49 && position_ < 0.51 && state_ == Idle)
			position_ = 0.5; // set the position to 0.5 to avoid floating point errors
	}
}


void Elevator::HandleOpened(Environment &env, const Event &e) {
	if (FromMe(e)) {
		isOpen_ = true;
	}
}

void Elevator::HandleClosed(Environment &env, const Event &e) {
	if (FromMe(e)){

	}
}

void Elevator::HandleBeep(Environment &env, const Event &e) {

	if (ForMe(e))
		beeping_ = env.SendEvent("Elevator::Beeping", 0, this);
}

void Elevator::HandleBeeping(Environment &env, const Event &e) {

	if (FromMe(e))
		beeping_ = 0;
}

void Elevator::HandleStopBeep(Environment &env, const Event &e) {

	if (ForMe(e)) {
		if (beeping_ != 0) {
			env.CancelEvent(beeping_);
			beeping_ = 0;
		} else
			env.SendEvent("Elevator::Beeped", 0, this);
	}
}

void Elevator::HandleMalfunction(Environment &env, const Event &e) {

	if (FromMe(e))
		state_ = Malfunction;
}

void Elevator::HandleFixed(Environment &env, const Event &e) {

	if (FromMe(e))
		state_ = Idle;
}
