/*
 * Event.h
 *
 *  Created on: 17.06.2014
 *      Author: STSJR
 */

#ifndef EVENT_H_
#define EVENT_H_

#include "Loadable.h"

#include <set>
#include <string>

class EventHandler;

class Event : public Loadable {
public:

	Event();
	virtual ~Event();

	const std::string &GetEvent() const;
	int GetTime() const;
	int GetId() const;
	EventHandler *GetSender() const;
	EventHandler *GetEventHandler() const;
	const std::string &GetData() const;

	void Load(Environment &env, std::istream &is);
	void Validate();

private:
	static std::set<std::string> events_;
	static std::set<std::string> loadables_;

	std::string type_;
	int time_;
	int id_;
	EventHandler *sender_;
	EventHandler *eventhandler_;
	std::string data_;
	bool loaded_;

	friend class Environment;
};

#endif /* EVENT_H_ */
