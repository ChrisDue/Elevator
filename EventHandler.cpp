/*
 * EventHandler.cpp
 *
 *  Created on: 02.07.2014
 *      Author: STSJR
 */

#include "EventHandler.h"

#include "Event.h"

EventHandler::EventHandler(const std::string &name) : name_(name) {
}

EventHandler::~EventHandler() {
}

bool EventHandler::ForMe(const Event &e) const {
	return (e.GetEventHandler() == static_cast<const EventHandler*>(this));
}

bool EventHandler::FromMe(const Event &e) const {
	return (e.GetSender() == static_cast<const EventHandler*>(this));
}

const std::string &EventHandler::GetName() const {
	return name_;
}


