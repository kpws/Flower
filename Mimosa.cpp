/*
 * Mimosa.cpp
 *
 *  Created on: Jul 14, 2009
 *      Author: petter
 */

#include "Mimosa.h"

Mimosa::Mimosa():menu(this,this,"Mimosa"),flowShower(this)
{
	// TODO Auto-generated constructor stub
}

Mimosa::~Mimosa()
{
	// TODO Auto-generated destructor stub
}

void Mimosa::init()
{
	menu.load();
	flowShower.load();

	menu.addItem("Play",Menu::IT_BUTTON);
	menu.addItem("Edit",Menu::IT_BUTTON);
	menu.addItem("Settings",Menu::IT_SUB);
	menu.addItem("Settings/Graphical settings",Menu::IT_SUB);
	menu.addItem("Settings/Graphical settings/Resolution",Menu::IT_BUTTON);
	menu.addItem("Exit",Menu::IT_BUTTON);

	setUI(&menu);
}

void Mimosa::menuEvent(string event)
{
	//cout << "event recieved: "<<event<<endl;

	if(event=="Mimosa/Exit")
		exit(0);
	if(event=="Mimosa/Play")
		setUI(&flowShower);
}

void Mimosa::UIEvent(string event)
{
	if(getUI()==&flowShower)
	{
		if(event=="exit")
			setUI(&menu);
	}
}
