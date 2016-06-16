/*
 * Interface.cpp
 *
 *  Created on: 17.06.2014
 *      Author: STSJR
 */

#include "Interface.h"

#include <iostream>
#include <stdexcept>

#include "Environment.h"

Interface::Interface() : Entity("Interface"), count_(0), loadables_(0) {
}

Interface::Interface(const std::string &name) : Entity(name), count_(0), loadables_(0) {
}

Interface::~Interface() {
	if (count_ > 0) {
		delete [] loadables_;
	}
}

int Interface::GetLoadableCount() const {
	return count_;
}

Loadable *Interface::GetLoadable(int n) const {
	return loadables_[n];
}

bool Interface::HasLoadable(Loadable *e) const {

	for (int i = 0; i < count_; ++i) {
		if (loadables_[i] == e)
			return true;
	}

	return false;
}

void Interface::DeclareEvents(Environment &env) {

	env.DeclareEvent("Interface::Interact");
	env.DeclareEvent("Interface::Notify");
}

void Interface::Initialize(Environment &env) {
	env.RegisterEventHandler("Interface::Interact", this, &Interface::HandleInteract);
}

void Interface::Load(Environment &env, std::istream &is) {

	Entity::Load(env, is);

	env.LoadInteger(&count_, is);

	loadables_ = new Loadable*[count_];

	for (int i = 0; i < count_; ++i) {
		env.LoadReference(&loadables_[i], is);
	}
}

void Interface::Validate() {

	Entity::Validate();

	if (count_ <= 0) {
		std::ostringstream oss("Number of referenced objects of ", std::ostringstream::ate);
		oss << GetName() << " must be positive.";
		throw TestfileException(oss.str());
	}

	for (int i = 0; i < count_; ++i) {

		Loadable *entity = loadables_[i];

		if (!entity) {
			std::ostringstream oss("An referenced object of ", std::ostringstream::ate);
			oss << GetName() << " is null.";
			throw TestfileException(oss.str());
		}
	}

}

void Interface::HandleInteract(Environment &env, const Event &e) {

	if (ForMe(e))
		env.SendEvent("Interface::Notify", 0, this, e.GetSender());
}

