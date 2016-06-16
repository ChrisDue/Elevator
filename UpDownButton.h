/*
 * UpDownButton.h
 *
 *  Created on: 18.06.2014
 *      Author: STSJR
 */

#ifndef UPDOWNBUTTON_H_
#define UPDOWNBUTTON_H_

#include "Interface.h"

class UpDownButton: public Interface {

public:

	UpDownButton();
	virtual ~UpDownButton();

	void DeclareEvents(Environment &env);
	void Initialize(Environment &env);

private:

	void HandleInteract(Environment &env, const Event &e);
	void HandleUp(Environment &env, const Event &e);
	void HandleDown(Environment &env, const Event &e);
};

#endif /* UPDOWNBUTTON_H_ */
