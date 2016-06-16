/*
 * Environment.h
 *
 *  Created on: 17.06.2014
 *      Author: STSJR
 */

#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include <list>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include "Entity.h"
#include "Event.h"
#include "DescriptionReader.h"
#include "LogicException.h"
#include "TestfileException.h"

class Environment : private Loadable {

public:
	Environment();
	virtual ~Environment();

	void Load(const std::string &file);
	void Run();
	inline int SendEvent(const std::string &type, int delay, EventHandler *src, const std::string &data) {
		return SendEvent(type, delay, src, nullptr, data);
	}
	int SendEvent(const std::string &type, int delay, EventHandler *src, EventHandler *tgt = nullptr, const std::string &data = "");
	bool CancelEvent(int id);

	int GetClock() const;

	void DeclareEvent(const std::string &name, bool loadable = false);
	template <typename T> void RegisterEventHandler(const std::string &type, T *obj, void (T::*fnc)(Environment &, const Event &)) {

		if (type != "Environment::All" && !Event::events_.count(type)) /* TODO Fehlertyp (SimulationException, TestfileException) ? */
				throw std::runtime_error("Invalid operation: cannot register event handler for unknown event type: " + type + ".");

		std::set<callback_t> set;

		auto iter = callbacks_.find(type);
		if (iter != callbacks_.end())
			set = iter->second;

		callback_t data;
		data.obj = reinterpret_cast<EventHandler*>(obj);
		data.callee = reinterpret_cast<void (EventHandler::*)(Environment &, const Event &)>(fnc);
		data.caller = &Environment::template CallEventHandler<T>;
		set.insert(data);

		callbacks_[type] = set;
	}

	void LoadString(std::string *dest, std::istream &is) const;
	void LoadParameters(std::string *dest, std::istream &is) const;
	void LoadInteger(int *dest, std::istream &is) const;
	void LoadDouble(double *dest, std::istream &is) const;
	void LoadBool(bool *dest, std::istream &is) const;
	void LoadReference(EventHandler **dest, std::istream &is) {

		reference_t ref;
		LoadInteger(&ref.id, is);

		if (ref.id < 0)
			throw TestfileException("Reference id must be non-negative.");

		if (ref.id > 0) {

			ref.ptr = reinterpret_cast<Loadable**>(dest);
			ref.resolver = &Environment::ResolveEventHandlerReference;

			references_.push_back(ref);
		}
	}
	template <typename T> void LoadReference(T **dest, std::istream &is) {

		reference_t ref;
		LoadInteger(&ref.id, is);

		if (ref.id < 0)
			throw TestfileException("Reference id must be non-negative.");

		if (ref.id > 0) {

			ref.ptr = reinterpret_cast<Loadable**>(dest);
			ref.resolver = &Environment::template ResolveReference<T>;

			references_.push_back(ref);
		}
	}

protected:

	template <typename T> void UseEntity() {
		T t;
		std::string type(t.GetType());
		std::cout << "Using Entity object: " << type << std::endl;
		creators_.insert(std::make_pair(type, &Environment::template CreateLoadable<T, Environment>));
		eventhandlernames_.insert(type);
		t.DeclareEvents(*this);
	}
	template <typename T, typename E> void UseEntity(void (E::*init)(T *) = 0) {
		T t;
		std::string type(t.GetType());
		std::cout << "Using Entity object: " << type << std::endl;
		creators_.insert(std::make_pair(type, &Environment::template CreateLoadable<T, E>));
		newhandlers_.insert(std::make_pair(type, reinterpret_cast<void (Environment::*)(Loadable*)>(init)));
		eventhandlernames_.insert(type);
		t.DeclareEvents(*this);
	}
	template <typename T> void UseLoadable() {
		std::string type(T().GetType());
		std::cout << "Using Loadable object: " << type << std::endl;
		creators_.insert(std::make_pair(type, &Environment::template CreateLoadable<T, Environment>));
	}
	template <typename T, typename E> void UseLoadable(void (E::*init)(T*) = 0) {
		std::string type(T().GetType());
		std::cout << "Using Loadable object: " << type << std::endl;
		creators_.insert(std::make_pair(type, &Environment::template CreateLoadable<T, E>));
		newhandlers_.insert(std::make_pair(type, reinterpret_cast<void (Environment::*)(Loadable*)>(init)));
	}
	void RegisterEventHandler(EventHandler *h);

	virtual void Evaluate() = 0;
	void ValidateIds();

private:
	void Integrate();
	template <typename T, typename E> Loadable *CreateLoadable() {

		T *e = new T;
		auto iter = newhandlers_.find(e->GetType());

		if (iter != newhandlers_.end())
			(reinterpret_cast<E*>(this)->*reinterpret_cast<void (E::*)(T*)>(iter->second))(e);

		return e;
	}
	template <typename E> void CallEventHandler(EventHandler *obj, void (EventHandler::*fnc)(Environment &, const Event &), const Event &e) {
		(reinterpret_cast<E*>(obj)->*reinterpret_cast<void (E::*)(Environment &, const Event &)>(fnc))(*this, e);
	}
	void ResolveEventHandlerReference(Loadable **ptr, int id) {

		Entity *entity = nullptr;
		for (Entity *cand : entities_) {
			if (cand->GetId() == id) {
				entity = cand;
			}
		}

		if (!entity) {
			std::ostringstream s("Unknown reference id: ",
					std::ostringstream::ate);
			s << id << ".";
			throw TestfileException(s.str());
		}

		*(reinterpret_cast<EventHandler**>(ptr)) = static_cast<EventHandler*>(entity);
	}

	template <typename T> void ResolveReference(Loadable **ptr, int id) {

		Loadable *loadable = nullptr;
		for (Loadable *cand : loadables_) {
			if (cand->GetId() == id) {
				loadable = cand;
			}
		}

		if (!loadable) {
			std::ostringstream s("Unknown reference id: ",
					std::ostringstream::ate);
			s << id << ".";
			throw TestfileException(s.str());
		}

		*(reinterpret_cast<T**>(ptr)) = static_cast<T*>(loadable);
	}

	void Load(Environment &env, std::istream &is);

	struct callback_t {
		EventHandler *obj;
		void (EventHandler::*callee)(Environment &, const Event &);
		void (Environment::*caller)(EventHandler*, void (EventHandler::*)(Environment &, const Event &), const Event &);

		bool operator<(const callback_t &r) const { return obj < r.obj; }
	};
	struct reference_t {
		int id;
		Loadable **ptr;
		void (Environment::*resolver)(Loadable **ptr, int id);
	};

	int clock_;
	std::list<Event*> queue_;
	std::vector<Event*> pending_;
	std::vector<reference_t> references_;
	std::map<const std::string, Loadable *(Environment::*)()> creators_;
	std::map<const std::string, void (Environment::*)(Loadable *)> newhandlers_;
	std::map<const std::string, std::set<callback_t>> callbacks_;
	std::map<long, Event*> ids_; // einer ID wird ein Event zugeordnet
	std::vector<Loadable*> loadables_;
	std::vector<EventHandler*> eventhandlers_;
	std::vector<Entity*> entities_;
	std::set<std::string> eventhandlernames_;
};

#endif /* ENVIRONMENT_H_ */
