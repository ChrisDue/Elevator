/*
 * UpDownButton.cpp
 *
 *  Created on: 18.06.2014
 *      Author: STSJR
 */

#include "UpDownButton.h"

#include <iostream>

#include "Environment.h"

UpDownButton::UpDownButton() : Interface("UpDownButton") {
}

UpDownButton::~UpDownButton() {
}

void UpDownButton::DeclareEvents(Environment &env) {

	Interface::DeclareEvents(env);
	env.DeclareEvent("UpDownButton::Decide");
	env.DeclareEvent("UpDownButton::Up");
	env.DeclareEvent("UpDownButton::Down");
}

void UpDownButton::Initialize(Environment &env) {
	env.RegisterEventHandler("Interface::Interact", this, &UpDownButton::HandleInteract);
	env.RegisterEventHandler("UpDownButton::Up", this, &UpDownButton::HandleUp);
	env.RegisterEventHandler("UpDownButton::Down", this, &UpDownButton::HandleDown);
}

void UpDownButton::HandleInteract(Environment &env, const Event &e) {

	if (ForMe(e))
		env.SendEvent("UpDownButton::Decide", 0, this, e.GetSender());
}

void UpDownButton::HandleUp(Environment &env, const Event &e) {

	if (ForMe(e))
		env.SendEvent("Interface::Notify", 0, this, e.GetSender(), "Up");
}

void UpDownButton::HandleDown(Environment &env, const Event &e) {

	if (ForMe(e))
		env.SendEvent("Interface::Notify", 0, this, e.GetSender(), "Down");
}
