/*
 * Mimosa.h
 *
 *  Created on: Jul 14, 2009
 *      Author: petter
 */

#ifndef MIMOSA_H_
#define MIMOSA_H_

#include "Game.h"
#include "Menu.h"
#include "FlowShower.h"


class Mimosa: public Game, public MenuEventReciever
{
public:
	Mimosa();
	virtual ~Mimosa();
	void menuEvent(string event);
	void UIEvent(string s);
protected:
	virtual void init();
private:
	Menu menu;
	FlowShower flowShower;
};

#endif /* MIMOSA_H_ */
