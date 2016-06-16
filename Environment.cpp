/*
 * Environment.cpp
 *
 *  Created on: 17.06.2014
 *      Author: STSJR
 */

#include "Environment.h"

#include <set>
#include <fstream>
#include <sstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <cstdlib>

#include "Entity.h"

Environment::Environment() : Loadable("Environment"), clock_(0) {
	UseLoadable<Event>();
}

Environment::~Environment() {
}

void Environment::Load(const std::string &file) {

	std::ifstream is(file);

	if (!is.is_open())
		throw std::runtime_error(
				"Error: could not open file: "
						+ file + ".");

	Load(*this, is);

	is.close();

	std::cout << "Test parsed successfully" << std::endl;

	for (reference_t ref : references_) {
		(this->*ref.resolver)(ref.ptr, ref.id);
	}

	references_.clear();

	std::cout << "Object references resolved" << std::endl;

	ValidateIds();

	for (Loadable *e : loadables_) {
		e->Validate();
	}

	std::cout << "Loaded objects validated" << std::endl;

	Validate();

	std::cout << "Environment validated" << std::endl;

	for (EventHandler *e : eventhandlers_) {
		e->Initialize(*this);
	}

	std::cout << "Event handlers initialized" << std::endl;
}

void Environment::Run() {

	Integrate();

	std::cout << "Starting simulation:" << std::endl;

	while (!queue_.empty()) {

		Event *e = queue_.front();


		if (e->GetTime() == clock_) {

			std::cout << "[" << e->GetTime() << "] " << e->GetSender()->GetName() << " sends " << e->GetEvent();

			if (e->GetEventHandler())
				std::cout << " referencing " << e->GetEventHandler()->GetName();

			if (e->GetData().length() > 0)
				std::cout << " (Data: " << e->GetData() << ")";

			std::cout << std::endl;

			queue_.pop_front();

			auto iter = callbacks_.find("Environment::All");
			if (iter != callbacks_.end()) {
				for (callback_t data : iter->second) {
					(this->*data.caller)(data.obj, data.callee, *e);
				}
			}


			iter = callbacks_.find(e->GetEvent());
			if (iter != callbacks_.end()) {
				for (callback_t data : iter->second) {
					(this->*data.caller)(data.obj, data.callee, *e);
				}
			}

			ids_.erase(e->GetId());
			delete e;

		} else {

			Integrate();

			e = queue_.front();

			if (e->GetTime() != clock_) {
				clock_ = e->GetTime();
			}
		}

		if (queue_.empty())
			Integrate();
	}

	std::cout << "Simulation finished." << std::endl;
}

int Environment::SendEvent(const std::string &type, int delay, EventHandler *src, EventHandler *tgt, const std::string &data) {

	// TODO Fehlermeldung erweitern?
	if (delay < 0)
		throw LogicException("Invalid operation: delay when sending events must be non-negative.");

	int id = rand();
	while (ids_.count(id))
		id = rand();

	Event *e = new Event;

	e->type_ = type;
	e->time_ = delay + clock_;
	e->sender_ = src;
	e->eventhandler_ = tgt;
	e->data_ = data;
	e->id_ = id;

	e->Validate();

	pending_.push_back(e);
	ids_.insert(std::make_pair(id, e));

	return id;
}

bool Environment::CancelEvent(int id) {

	auto pair = ids_.find(id);

	if (pair == ids_.end())
		return false;

	Event *e = pair->second;
	ids_.erase(pair);

	for (auto iter = pending_.begin(); iter != pending_.end(); ++iter) {
		if (*iter == e) {
			pending_.erase(iter);
			return true;
		}
	}
	for (auto iter = queue_.begin(); iter != queue_.end(); ++iter) {
		if (*iter == e) {
			queue_.erase(iter);
			return true;
		}
	}
	return false;
}

int Environment::GetClock() const {
	return clock_;
}

void Environment::DeclareEvent(const std::string &name, bool loadable) {

	auto pair = Event::events_.insert(name);

	if (pair.second) {

		if (loadable) {
			Event::loadables_.insert(name);

			std::cout << "Declared loadable event: " << name << std::endl;

		} else
			std::cout << "Declared event: " << name << std::endl;
	}
}

void Environment::LoadString(std::string *dest, std::istream &is) const {

	std::string ret;

	if (!is.eof())
		is >> ret;

	*dest = ret;
}

void Environment::LoadParameters(std::string *dest, std::istream &is) const {

	if (is.eof())
		throw TestfileException(
				"Expected '{', but sequence is empty.");

	std::ostringstream ret;
	std::string s;
	is >> s;

	if (s.size() != 1 || *s.begin() != '{')
		throw TestfileException("Expected '{'.");

	int o = 1;
	bool first = true;
	while (o > 0) {

		if (is.eof())
			throw TestfileException(
					"Expected '}', but sequence is empty.");

		is >> s;

		if (s.size() == 1 && *s.begin() == '{')
			++o;
		else if (s.size() == 1 && *s.begin() == '}')
			--o;
		else {
			if (!first)
				ret << ' ';
			ret << s;
			first = false;
		}
	}

	*dest = ret.str();
}

void Environment::LoadInteger(int *dest, std::istream &is) const {

	if (is.eof())
		throw TestfileException(
				"Expected integer, but sequence is empty.");

	int i;
	is >> i;

	if (is.fail()) {
		is.clear();
		std::string s;
		is >> s;
		throw TestfileException("Expected integer: " + s + ".");
	}

	*dest = i;
}

void Environment::LoadDouble(double *dest, std::istream &is) const {

	if (is.eof())
		throw TestfileException(
				"Expected double, but sequence is empty.");

	double d;
	is >> d;

	if (is.fail()) {
		is.clear();
		std::string s;
		is >> s;
		throw TestfileException("Expected double: " + s + ".");
	}

	*dest = d;
}

void Environment::LoadBool(bool *dest, std::istream &is) const {

	if (is.eof())
		throw TestfileException(
				"Expected boolean, but sequence is empty.");

	bool b;
	is >> b;

	if (is.fail()) {
		is.clear();
		std::string s;
		is >> s;
		throw TestfileException("Expected boolean: " + s + ".");
	}

	*dest = b;
}

void Environment::RegisterEventHandler(EventHandler *h) {
	eventhandlers_.push_back(h);
}

void Environment::ValidateIds() {

	std::set<int> unique;
	for (Loadable *e : loadables_) {

		if (e->GetType() == "Event")
			continue;

		if (unique.count(e->GetId())) {
			std::ostringstream s("Duplicate object id: ", std::ostringstream::ate);
			s << e->GetId() << ".";
			throw TestfileException(s.str());
		}
		unique.insert(e->GetId());
	}
}

void Environment::Integrate() {

	for (Event *e : pending_) {

		auto iter = queue_.begin();
		while (iter != queue_.end() && (*iter)->GetTime() <= e->GetTime())
			++iter;

		queue_.insert(iter, e);
	}

	pending_.clear();
}

void Environment::Load(Environment &env, std::istream &is) {

	while (!is.eof()) {

		std::string loadable;
		std::string paras;

		LoadString(&loadable, is);

		if (loadable.size() == 0)
			break;

		if (loadable == "//") {
			is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			continue;
		}

		if (loadable == "Description") {

			DescriptionReader::IgnoreParameters(is);
			continue;
		}

		auto creator = creators_.find(loadable);

		if (creator == creators_.end())
			throw TestfileException("Unknown object type: " + loadable + ".");

		LoadParameters(&paras, is);

		std::istringstream ps(paras);

		Loadable *e = (this->*creator->second)();

		if (!e)
			throw TestfileException(
							"Loading error: could not create object: "
							+ loadable + ", this error should never occur.");

		e->Load(env, ps);

		std::cout << "Created " << loadable << " " << e->GetId() << std::endl;

		if (!ps.eof()) {
			std::string rest;
			std::getline(ps, rest);
			throw TestfileException(
					"Unexpected parameter(s): " + rest + ".");
		}

		loadables_.push_back(e);

		if (eventhandlernames_.count(loadable)) {

			Entity *f = static_cast<Entity*>(e);
			entities_.push_back(f);
			RegisterEventHandler(f);

		} else if (loadable == "Event") {

			Event *f = static_cast<Event*>(e);
			pending_.push_back(f);
		}
	}
}

