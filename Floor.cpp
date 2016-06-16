/*
 * Floor.cpp
 *
 *  Created on: 18.06.2014
 *      Author: STSJR
 */

#include "Floor.h"

#include <set>

#include "Environment.h"
#include "Elevator.h"
#include "Interface.h"

Floor::Floor() :
		Loadable("Floor"), below_(0), above_(0), height_(0), count_(0), interfs_(0) {
}

Floor::~Floor() {
	if (count_ > 0) {
		delete [] interfs_;
	}
}

bool Floor::HasElevator(Elevator *ele) const {

	for (int i = 0; i < count_; ++i) {

		Interface *interf = interfs_[i];

		if (interf->HasLoadable(ele))
			return true;
	}

	return false;
}

bool Floor::IsAbove(Floor *floor) const {

	Floor *above = above_;

	while (above) {

		if (above == floor)
			return true;

		above = above->GetAbove();
	}

	return false;
}

bool Floor::IsBelow(Floor *floor) const {

	Floor *below = below_;

	while (below) {

		if (below == floor)
			return true;

		below = below->GetBelow();
	}

	return false;
}

Floor *Floor::GetBelow() const {
	return below_;
}

Floor *Floor::GetAbove() const {
	return above_;
}

int Floor::GetHeight() const {
	return height_;
}

int Floor::GetInterfaceCount() const {
	return count_;
}

Interface *Floor::GetInterface(int n) const {
	return interfs_[n];
}

void Floor::Load(Environment &env, std::istream &is) {

	Loadable::Load(env, is);

	env.LoadReference(&below_, is);
	env.LoadReference(&above_, is);
	env.LoadInteger(&height_, is);
	env.LoadInteger(&count_, is);

	interfs_ = new Interface*[count_];
	for (int i = 0; i < count_; ++i) {
		env.LoadReference(&interfs_[i], is);
	}
}

void Floor::Validate() {

	Loadable::Validate();

	if (below_) {

		if (below_->GetType() != "Floor") {
			std::ostringstream oss("Floor below of Floor ", std::ostringstream::ate);
			oss << GetId() << " must reference a floor object.";
			throw TestfileException(oss.str());
		}

		if (below_->GetAbove() != this) {
			std::ostringstream oss("Floor below of Floor ", std::ostringstream::ate);
			oss << GetId() << " does not point back to the floor.";
			throw TestfileException(oss.str());
		}
	}
	if (above_) {
		if (above_->GetType() != "Floor") {
			std::ostringstream oss("Floor above of Floor ", std::ostringstream::ate);
			oss << GetId() << " must reference a floor object.";
			throw TestfileException(oss.str());
		}

		if (above_->GetBelow() != this) {
			std::ostringstream oss("Floor above of Floor ", std::ostringstream::ate);
			oss << GetId() << " does not point back to the floor.";
			throw TestfileException(oss.str());
		}
	}

	if (height_ <= 0) {
		std::ostringstream oss("Height of Floor ", std::ostringstream::ate);
		oss << GetId() << " must be positive.";
		throw TestfileException(oss.str());
	}

	if (count_ < 0) {
		std::ostringstream oss("Interface count of Floor ", std::ostringstream::ate);
		oss << GetId() << " must be non-negative.";
		throw TestfileException(oss.str());
	}

	std::set<Interface*> unique;

	for (int i = 0; i < count_; ++i) {

		Interface *interf = interfs_[i];

		if (!interf) {
			std::ostringstream oss("An interface of Floor ", std::ostringstream::ate);
			oss << GetId() << " is null.";
			throw TestfileException(oss.str());
		}

		if (unique.count(interf)) {
			std::ostringstream oss("Interfaces used by Floor ", std::ostringstream::ate);
			oss << GetId() << " are not unique.";
			throw TestfileException(oss.str());
		}
		unique.insert(interf);

		std::set<Loadable*> reference;

		for (int j = 0; j < interf->GetLoadableCount(); ++j) {

			Loadable *loadable = interf->GetLoadable(j);

			if (!loadable) {
				std::ostringstream oss("An object referenced by Interface ", std::ostringstream::ate);
				oss << interf->GetId() << " of Floor ";
				oss << GetId() << " is null.";
				throw TestfileException(oss.str());
			}

			if (loadable->GetType() != "Elevator") {
				std::ostringstream oss("An object referenced by Interface ", std::ostringstream::ate);
				oss << interf->GetId() << " of Floor ";
				oss << GetId() << " is not an elevator object.";
				throw TestfileException(oss.str());
			}

			Elevator *ele = static_cast<Elevator*>(loadable);

			std::set<Loadable*> loadables;

			for (int k = 0; k < ele->GetInterfaceCount(); ++k) {

				Interface *binterf = ele->GetInterface(k);

				if (!binterf) {
					std::ostringstream oss("An interface of ", std::ostringstream::ate);
					oss << ele->GetName() << " is null.";
					throw TestfileException(oss.str());
				}

				if (binterf->GetLoadableCount() != 1) {
					std::ostringstream oss("", std::ostringstream::ate);
					oss << binterf->GetName() << " of ";
					oss << ele->GetName() << " does not reference exactly one entity.";
					throw TestfileException(oss.str());
				}

				Loadable *bloadable = binterf->GetLoadable(0);

				if (!bloadable) {
					std::ostringstream oss("An object referenced by ", std::ostringstream::ate);
					oss << binterf->GetName() << " of ";
					oss << ele->GetName() << " is null.";
					throw TestfileException(oss.str());
				}

				loadables.insert(bloadable);
			}
			if (reference.size() == 0) {
				reference = loadables;

				if (!reference.count(this)) {
					std::ostringstream oss("", std::ostringstream::ate);
					oss << ele->GetName() << " of Floor ";
					oss << GetId() << " does not point back to the floor.";
					throw TestfileException(oss.str());
				}

			} else if (reference != loadables) {
				std::ostringstream oss("Elevators called by Interface ", std::ostringstream::ate);
				oss << interf->GetId() << " of Floor ";
				oss << GetId() << " stop at different floors.";
				throw TestfileException(oss.str());
			}
		}

		bool top = true;
		bool bottom = true;
		for (Loadable *loadable : reference) {

			Floor *floor = static_cast<Floor*>(loadable);

			if (IsAbove(floor))
				top = false;
			if (IsBelow(floor))
				bottom = false;
		}

		if (!top && !bottom) {
			if (interf->GetType() != "UpDownButton") {
				std::ostringstream oss("Interface ", std::ostringstream::ate);
				oss << interf->GetId() << " of Floor";
				oss << GetId() << " must be an UpDownButton object as the floor is an intermediate floor for the called elevators.";
				throw TestfileException(oss.str());
			}
		}
		else if (interf->GetType() != "Interface") {
			std::ostringstream oss("Interface ", std::ostringstream::ate);
			oss << interf->GetId() << " of Floor ";
			oss << GetId() << " must be an Interface object as the floor is a top or bottom floor for the called elevators.";
			throw TestfileException(oss.str());
		}
	}
}

