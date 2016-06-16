/*
 * EventHandler.h
 *
 *  Created on: 20.06.2014
 *      Author: STSJR
 */

#ifndef EVENTHANDLER_H_
#define EVENTHANDLER_H_

#include <string>

class Event;
class Environment;

class EventHandler {

public:
	EventHandler(const std::string &name);
	virtual ~EventHandler();

	bool ForMe(const Event &e) const;
	bool FromMe(const Event &e) const;
	const std::string &GetName() const;

	virtual void DeclareEvents(Environment &env) { }
	virtual void Initialize(Environment &env) = 0;

protected:
	std::string name_;
};

#endif /* EVENTHANDLER_H_ */
