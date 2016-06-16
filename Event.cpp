/*
 * Event.cpp
 *
 *  Created on: 17.06.2014
 *      Author: STSJR
 */

#include "Event.h"

#include "Environment.h"

std::set<std::string> Event::events_;
std::set<std::string> Event::loadables_;

Event::Event() : Loadable("Event"), type_("Undefined"), time_(0), id_(0), sender_(0), eventhandler_(0), data_(), loaded_(false) {
}

Event::~Event() {
}

const std::string &Event::GetEvent() const {
	return type_;
}

int Event::GetTime() const {
	return time_;
}

int Event::GetId() const {
	return id_;
}

EventHandler *Event::GetSender() const {
	return sender_;
}

EventHandler *Event::GetEventHandler() const {
	return eventhandler_;
}

const std::string &Event::GetData() const {
	return data_;
}

void Event::Load(Environment &env, std::istream &is) {

	loaded_ = true;

	env.LoadString(&type_, is);
	env.LoadInteger(&time_, is);
	env.LoadReference(&sender_, is);
	env.LoadReference(&eventhandler_, is);
	env.LoadString(&data_, is);
}

void Event::Validate() {

	if (!events_.count(type_))
		throw TestfileException("Unknown event type: " + type_ + ".");

	if (loaded_)
		if (!loadables_.count(type_))
			throw TestfileException("The event type " + type_ + " may not be loaded.");

	if (time_ < 0)
		throw TestfileException("The event (type " + type_ + ") time must be positive.");

}
