/*
 * Editor.h
 *
 *  Created on: Jul 26, 2009
 *      Author: petter
 */

#ifndef EDITOR_H_
#define EDITOR_H_

#include <time.h>

#include "UserInterface.h"
#include "Shader.h"
#include "Fbo.h"
#include "Flower.h"
#include "Obstacligizer.h"

class FlowShower: public UserInterface, Obstacligizer
{
public:
	FlowShower(Game *g);
	void load();
	void init();
	void draw();
	void keyPress(char key);
	void mouseClick(int button, int state);
	void mouseMove(int x,int y);
	void update();
	void obstacligize(float dt);
	virtual ~FlowShower();

	static void getSpeed(int size,float *v,float x,float y);
	static void getPressure(int size,float *p,float x,float y);
private:
	Flower flower;
	Shader 	dispInkMat,dispCoordInkMat,dispMatMat,dispRhoMat,dispRotMat,dispDivMat,initFromFGMat;
	int size,inksize;
	clock_t lastClock;
	float dt;
	int iterations;
	int flowerTex;
	float oldMouseX,oldMouseOSX,oldMouseY,scale,scaleb,viewX,viewY,pressX,pressY,pressViewX,pressViewY;
	bool showSides,painting,streamlining,coordInk,showMat,showRho,showRot,showDiv,paused;
	float inkRed,inkGreen,inkBlue,inkRedNew,inkGreenNew,inkBlueNew,inkNew;
	float oldx1;
	float oldx2;
	float oldy1;
	float oldy2;
	const static int slpnum = 80;
	float slx[slpnum], sly[slpnum];

	void drawMouseOffScreen();
	void displayResult();
	void drawViewQuad();
};

#endif /* EDITOR_H_ */
