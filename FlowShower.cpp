/*
 * Editor.cpp
 *
 *  Created on: Jul 26, 2009
 *      Author: petter
 */

#include <GL/glut.h>
#include <math.h>
#include <GL/glpng.h>

#include "FlowShower.h"
#include "TexGroup.h"

FlowShower::FlowShower(Game*g) :
	UserInterface(g)
{
	dt = 0;
	lastClock = 0;
	showSides = true;
	showRho = false;
	showMat = true;
	showRot = false;
	showDiv = false;
	painting = false;
	streamlining = false;
	coordInk = false;
	inkNew=1.0f;
	inkRedNew=0.0f;
	inkGreenNew=0.0f;
	inkBlueNew=0.0f;
}
FlowShower::~FlowShower()
{
	// TODO Auto-generated destructor stub
}
void FlowShower::load()
{
	paused = false;
	size = 512;
	inksize = 2048;

	flower.load(size, inksize);

	dispCoordInkMat.load("dispCoordInk.frag", "orth.vert");
	dispCoordInkMat.enable();
	dispCoordInkMat.sendInt(dispCoordInkMat.getLocation("inkTex"), 1);
	dispCoordInkMat.sendInt(dispCoordInkMat.getLocation("flowerTex"), 2);
	dispCoordInkMat.disable();

	dispInkMat.load("dispInk.frag", "orth.vert");
	dispInkMat.enable();
	dispInkMat.sendInt(dispInkMat.getLocation("inkTex"), 1);
	dispInkMat.disable();

	dispMatMat.load("dispMat.frag", "orth.vert");
	dispMatMat.enable();
	dispMatMat.sendInt(dispMatMat.getLocation("res"), size);
	dispMatMat.sendInt(dispMatMat.getLocation("matTex"), 0);
	dispMatMat.disable();

	dispRhoMat.load("dispRho.frag", "orth.vert");
	dispRhoMat.enable();
	dispRhoMat.sendInt(dispRhoMat.getLocation("res"), size);
	dispRhoMat.sendInt(dispRhoMat.getLocation("flowTex"), 0);
	dispRhoMat.disable();

	dispRotMat.load("dispRot.frag", "orth.vert");
	dispRotMat.enable();
	dispRotMat.sendInt(dispRotMat.getLocation("res"), size);
	dispRotMat.sendInt(dispRotMat.getLocation("flowTex"), 0);
	dispRotMat.disable();

	dispDivMat.load("dispDiv.frag", "orth.vert");
	dispDivMat.enable();
	dispDivMat.sendInt(dispDivMat.getLocation("res"), size);
	dispDivMat.sendInt(dispDivMat.getLocation("flowTex"), 0);
	dispDivMat.disable();

	initFromFGMat.load("initFromfg.frag", "orth.vert");
	initFromFGMat.enable();
	initFromFGMat.sendInt(initFromFGMat.getLocation("fgTex"), 0);
	initFromFGMat.disable();

	flowerTex = TexGroup::getSingleton()->add("flower.png");

	iterations = 15;

	scale = 1.0f;
	scaleb = 1.0f;
	viewX = 0.5f;
	viewY = 0.5f;
}
void FlowShower::init()
{
	game->centerMouse(false);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	oldMouseX = float(game->getMouseX()) / float(game->getResX());
	oldMouseY = 1.0f - float(game->getMouseY()) / float(game->getResY());
	float invRatio = float(game->getResY()) / float(game->getResX());
	float left = (1.0f - invRatio) / 2.0f;
	float right = 1.0f - left;
	oldMouseOSX = (oldMouseX - left) / (right - left);
}
void FlowShower::getSpeed(int size, float *speed, float x, float y)
{
	int xs = int(x * float(size));
	int ys = int(y * float(size));
	float u = x * float(size) - float(xs);
	float v = y * float(size) - float(ys);

	float pixel[3 * 2 * 2];
	if (xs >= size - 2 || ys >= size - 2)
	{
		glReadPixels((xs + 0) % size, (ys + 0) % size, 1, 1, GL_RGB, GL_FLOAT,
				(void *) (&pixel[0 * 3]));
		glReadPixels((xs + 1) % size, (ys + 0) % size, 1, 1, GL_RGB, GL_FLOAT,
				(void *) (&pixel[1 * 3]));
		glReadPixels((xs + 0) % size, (ys + 1) % size, 1, 1, GL_RGB, GL_FLOAT,
				(void *) (&pixel[2 * 3]));
		glReadPixels((xs + 1) % size, (ys + 1) % size, 1, 1, GL_RGB, GL_FLOAT,
				(void *) (&pixel[3 * 3]));
	}
	else
		glReadPixels(xs, ys, 2, 2, GL_RGB, GL_FLOAT, (void *) pixel);

	speed[0] = (pixel[0 * 3] * (1.0f - u) + pixel[1 * 3] * u) * (1.0f - v)
			+ (pixel[2 * 3] * (1.0f - u) + pixel[3 * 3] * u) * v;
	speed[1] = (pixel[0 * 3 + 1] * (1.0f - u) + pixel[1 * 3 + 1] * u) * (1.0f
			- v) + (pixel[2 * 3 + 1] * (1.0f - u) + pixel[3 * 3 + 1] * u) * v;
}
void FlowShower::getPressure(int size, float *p, float x, float y)
{
	while (x < 0.0f)
		x += 1.0f;
	while (y < 0.0f)
		y += 1.0f;
	int xs = int(x * float(size));
	int ys = int(y * float(size));
	float u = x * float(size) - float(xs);
	float v = y * float(size) - float(ys);

	float pixel[3 * 2 * 2];
	if (xs >= size - 2 || ys >= size - 2)
	{
		glReadPixels((xs + 0) % size, (ys + 0) % size, 1, 1, GL_RGB, GL_FLOAT,
				(void *) (&pixel[0 * 3]));
		glReadPixels((xs + 1) % size, (ys + 0) % size, 1, 1, GL_RGB, GL_FLOAT,
				(void *) (&pixel[1 * 3]));
		glReadPixels((xs + 0) % size, (ys + 1) % size, 1, 1, GL_RGB, GL_FLOAT,
				(void *) (&pixel[2 * 3]));
		glReadPixels((xs + 1) % size, (ys + 1) % size, 1, 1, GL_RGB, GL_FLOAT,
				(void *) (&pixel[3 * 3]));
	}
	else
		glReadPixels(xs, ys, 2, 2, GL_RGB, GL_FLOAT, (void *) pixel);

	p[0] = (pixel[0 * 3 + 2] * (1.0f - u) + pixel[1 * 3 + 2] * u) * (1.0f - v)
			+ (pixel[2 * 3 + 2] * (1.0f - u) + pixel[3 * 3 + 2] * u) * v;
}
void FlowShower::drawMouseOffScreen()
{
	 /*float invRatio = float(game->getResY()) / float(game->getResX());
	 float hs = 0.02f; //mouse horizontal size
	 float vs = hs * invRatio; //mouse vertical size
	 float x = float(game->getMouseX()) / float(game->getResX());
	 float y = 1.0f - float(game->getMouseY()) / float(game->getResY());
	 float left = (1.0f - invRatio) / 2.0f;
	 float right = 1.0f - left;
	 float osx = (x - left) / (right - left);

	 osx -= 1.0f;
	 oldMouseOSX -= 1.0f;
	 for (int j = 0; j < 3; j++)
	 {
	 glBegin(GL_QUADS);

	 glTexCoord2f(osx - hs, y - hs);
	 glVertex2f(osx - hs, y - hs);
	 glTexCoord2f(osx + hs, y - hs);
	 glVertex2f(osx + hs, y - hs);
	 glTexCoord2f(osx + hs, y + hs);
	 glVertex2f(osx + hs, y + hs);
	 glTexCoord2f(osx - hs, y + hs);
	 glVertex2f(osx - hs, y + hs);

	 glTexCoord2f(osx + hs, y - hs);
	 glVertex2f(osx + hs, y - hs);
	 glTexCoord2f(osx - hs, y - hs);
	 glVertex2f(osx - hs, y - hs);
	 glTexCoord2f(oldMouseOSX - hs, oldMouseY - hs);
	 glVertex2f(oldMouseOSX - hs, oldMouseY - hs);
	 glTexCoord2f(oldMouseOSX + hs, oldMouseY - hs);
	 glVertex2f(oldMouseOSX + hs, oldMouseY - hs);

	 glTexCoord2f(osx + vs, y + hs);
	 glVertex2f(osx + vs, y + hs);
	 glTexCoord2f(osx + vs, y - hs);
	 glVertex2f(osx + vs, y - hs);
	 glTexCoord2f(oldMouseOSX + hs, oldMouseY - hs);
	 glVertex2f(oldMouseOSX + hs, oldMouseY - hs);
	 glTexCoord2f(oldMouseOSX + hs, oldMouseY + hs);
	 glVertex2f(oldMouseOSX + hs, oldMouseY + hs);

	 glTexCoord2f(osx - vs, y + hs);
	 glVertex2f(osx - vs, y + hs);
	 glTexCoord2f(osx + vs, y + hs);
	 glVertex2f(osx + vs, y + hs);
	 glTexCoord2f(oldMouseOSX + hs, oldMouseY + hs);
	 glVertex2f(oldMouseOSX + hs, oldMouseY + hs);
	 glTexCoord2f(oldMouseOSX - hs, oldMouseY + hs);
	 glVertex2f(oldMouseOSX - hs, oldMouseY + hs);

	 glTexCoord2f(osx - vs, y - hs);
	 glVertex2f(osx - vs, y - hs);
	 glTexCoord2f(osx - vs, y + hs);
	 glVertex2f(osx - vs, y + hs);
	 glTexCoord2f(oldMouseOSX - hs, oldMouseY + hs);
	 glVertex2f(oldMouseOSX - hs, oldMouseY + hs);
	 glTexCoord2f(oldMouseOSX - hs, oldMouseY - hs);
	 glVertex2f(oldMouseOSX - hs, oldMouseY - hs);

	 glEnd();
	 osx += 1.0f;
	 oldMouseOSX += 1.0f;
	 }
	 osx -= 2.0f;
	 oldMouseOSX -= 2.0f;*/
	float sx = float(game->getMouseX()) / float(game->getResY());
	float sy = 1.0f - float(game->getMouseY()) / float(game->getResY());
	float x = (sx-0.5* float(game->getResX())/ float(game->getResY()))/scale+ viewX;
	float y = (sy-0.5)/scale+ viewY;
	if(x>0)
		x=x-static_cast<int>(x);
	else
		x=x-static_cast<int>(x)+1.0f;
	if(y>0)
		y=y-static_cast<int>(y);
	else
		y=y-static_cast<int>(y)+1.0f;
	float s = 0.02/scale;

	float x1=x - s;
	float x2=x + s;
	float y1=y - s;
	float y2=y + s;

	glBegin(GL_QUADS);

	glTexCoord2f(x1,y1);
	glVertex2f(x1,y1);
	glTexCoord2f(x2, y1);
	glVertex2f(x2, y1);
	glTexCoord2f(x2, y2);
	glVertex2f(x2, y2);
	glTexCoord2f(x1, y2);
	glVertex2f(x1, y2);

	glTexCoord2f(x1,y1);
	glVertex2f(x1,y1);
	glTexCoord2f(x2, y1);
	glVertex2f(x2, y1);

	glTexCoord2f(oldx2, oldy1);
	glVertex2f(oldx2, oldy1);

	glTexCoord2f(oldx1,oldy1);
	glVertex2f(oldx1,oldy1);


	glTexCoord2f(x2, y1);
	glVertex2f(x2, y1);
	glTexCoord2f(x2, y2);
	glVertex2f(x2, y2);
	glTexCoord2f(oldx2, oldy2);
	glVertex2f(oldx2, oldy2);
	glTexCoord2f(oldx2, oldy1);
	glVertex2f(oldx2, oldy1);



	glTexCoord2f(x2, y2);
	glVertex2f(x2, y2);
	glTexCoord2f(x1, y2);
	glVertex2f(x1, y2);
	glTexCoord2f(oldx1, oldy2);
	glVertex2f(oldx1,oldy2);
	glTexCoord2f(oldx2, oldy2);
	glVertex2f(oldx2, oldy2);



	glTexCoord2f(x1, y2);
	glVertex2f(x1, y2);
	glTexCoord2f(x1,y1);
	glVertex2f(x1,y1);
	glTexCoord2f(oldx1,oldy1);
	glVertex2f(oldx1,oldy1);
	glTexCoord2f(oldx1, oldy2);
	glVertex2f(oldx1, oldy2);

	glEnd();
}
void FlowShower::displayResult()
{
	float invRatio = float(game->getResY()) / float(game->getResX());
	float hs = 0.02f; //mouse horizontal size
	float vs = hs * invRatio; //mouse vertical size
	float x = float(game->getMouseX()) / float(game->getResX());
	float y = 1.0f - float(game->getMouseY()) / float(game->getResY());
	float left = (1.0f - invRatio) / 2.0f;
	float right = 1.0f - left;
	float osx = (x - left) / (right - left);

	glActiveTextureARB(GL_TEXTURE2_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	TexGroup::getSingleton()->bind(flower.getFlowTex(), 0);
	TexGroup::getSingleton()->bind(flower.getInkTex(), 1);
	if(coordInk)
	{
		TexGroup::getSingleton()->bind(flowerTex, 2);
		dispCoordInkMat.enable();
	}
	else
	{
		dispInkMat.enable();
	}

	drawViewQuad();

	if (showMat)
	{
		glActiveTextureARB(GL_TEXTURE2_ARB);
		glDisable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE0_ARB);
		glEnable(GL_TEXTURE_2D);
		TexGroup::getSingleton()->bind(flower.getMatTex(), 0);
		dispMatMat.enable();
		drawViewQuad();
	}
	if (showRho)
	{
		TexGroup::getSingleton()->bind(flower.getFlowTex(), 0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		dispRhoMat.enable();
		drawViewQuad();
		glDisable(GL_BLEND);
	}
	if (showRot)
	{
		TexGroup::getSingleton()->bind(flower.getFlowTex(), 0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		dispRotMat.enable();
		drawViewQuad();
		glDisable(GL_BLEND);
	}
	if (showDiv)
	{
		TexGroup::getSingleton()->bind(flower.getFlowTex(), 0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		dispDivMat.enable();
		drawViewQuad();
		glDisable(GL_BLEND);
	}

	Shader::disable();

	glActiveTextureARB(GL_TEXTURE2_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if(painting)
		glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
	else
		glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
	glBegin(GL_QUADS);
	glVertex2f(x - vs, y - hs);
	glVertex2f(x + vs, y - hs);
	glVertex2f(x + vs, y + hs);
	glVertex2f(x - vs, y + hs);

	glVertex2f(x + vs, y - hs);
	glVertex2f(x - vs, y - hs);
	glVertex2f(oldMouseX - vs, oldMouseY - hs);
	glVertex2f(oldMouseX + vs, oldMouseY - hs);

	glVertex2f(x + vs, y + hs);
	glVertex2f(x + vs, y - hs);
	glVertex2f(oldMouseX + vs, oldMouseY - hs);
	glVertex2f(oldMouseX + vs, oldMouseY + hs);

	glVertex2f(x - vs, y + hs);
	glVertex2f(x + vs, y + hs);
	glVertex2f(oldMouseX + vs, oldMouseY + hs);
	glVertex2f(oldMouseX - vs, oldMouseY + hs);

	glVertex2f(x - vs, y - hs);
	glVertex2f(x - vs, y + hs);
	glVertex2f(oldMouseX - vs, oldMouseY + hs);
	glVertex2f(oldMouseX - vs, oldMouseY - hs);
	glEnd();
	glDisable(GL_BLEND);
	oldMouseX = x;
	oldMouseOSX = osx;
	oldMouseY = y;

	if (streamlining)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < slpnum; i++)
		{
			float x =(slx[i]/ float(game->getResX())* float(game->getResY())-viewX/ float(game->getResX())* float(game->getResY()))*scale+0.5;
			float y =(sly[i]-viewY)*scale+0.5;

			glVertex2f(x,y);

		}
		glEnd();



		glDisable(GL_BLEND);
	}

}
void FlowShower::drawViewQuad()
{
	glBegin(GL_QUADS);
	float halfWidth = 1.0f / float(game->getResY()) * float(game->getResX())
			/ scale / 2.0f;
	float halfHeight = 1.0f / scale / 2.0f;
	glTexCoord2f(viewX - halfWidth, viewY - halfHeight);
	glVertex2f(0.0f, 0.0f);
	glTexCoord2f(viewX + halfWidth, viewY - halfHeight);
	glVertex2f(1.0f, 0.0f);
	glTexCoord2f(viewX + halfWidth, viewY + halfHeight);
	glVertex2f(1.0f, 1.0f);
	glTexCoord2f(viewX - halfWidth, viewY + halfHeight);
	glVertex2f(0.0f, 1.0f);
	glEnd();
}
void FlowShower::obstacligize(float dt)
{
	if (game->isLeftDown() && !painting)
	{

		float invRatio = float(game->getResY()) / float(game->getResX());
		float x = float(game->getMouseX()) / float(game->getResX());
		float y = 1.0f - float(game->getMouseY()) / float(game->getResY());
		float left = (1.0f - invRatio) / 2.0f;
		float right = 1.0f - left;
		float osx = (x - left) / (right - left);

		flower.sendObstacleInfo(0.0f, 0.0f, (osx - oldMouseOSX) / dt
				/ float(iterations)/scale, (y - oldMouseY) / dt /float( iterations)/scale, 0.0f);
		//cout << (osx - oldMouseOSX)<<endl;
		drawMouseOffScreen();
	}
}
void FlowShower::draw()
{
	update();
	setOrthoView();
	glClear(GL_COLOR_BUFFER_BIT);

	float invRatio = float(game->getResY()) / float(game->getResX());
	float hs = 0.02f; //mouse horizontal size
	//float vs = hs * invRatio; //mouse vertical size
	float x = float(game->getMouseX()) / float(game->getResX());
	float y = 1.0f - float(game->getMouseY()) / float(game->getResY());
	float left = (1.0f - invRatio) / 2.0f;
	float right = 1.0f - left;
	float osx = (x - left) / (right - left);

	if (painting && (game->isLeftDown() || game->isMiddleDown()
			|| game->isKeyDown('n') || game->isRightDown()))
	{
		flower.bindMat();

		glDisable(GL_TEXTURE_2D);

		if (game->isLeftDown())
			glColor3f(0.1, 0.0f, 0.0f);
		else if (game->isMiddleDown() || game->isKeyDown('m'))
			glColor3f(0.2, 0.0f, 0.0f);
		else if (game->isRightDown())
			glColor3f(0.0, 0.0f, 0.0f);

		/*glBegin(GL_QUADS);
		osx -= 1.0f;
		oldMouseOSX -= 1.0f;
		for (int j = 0; j < 3; j++)
		{
			glVertex2f(osx - hs, y - hs);
			glVertex2f(osx + hs, y - hs);
			glVertex2f(osx + hs, y + hs);
			glVertex2f(osx - hs, y + hs);

			glVertex2f(osx + hs, y - hs);
			glVertex2f(osx - hs, y - hs);
			glVertex2f(oldMouseOSX - hs, oldMouseY - hs);
			glVertex2f(oldMouseOSX + hs, oldMouseY - hs);

			glVertex2f(osx + vs, y + hs);
			glVertex2f(osx + vs, y - hs);
			glVertex2f(oldMouseOSX + hs, oldMouseY - hs);
			glVertex2f(oldMouseOSX + hs, oldMouseY + hs);

			glVertex2f(osx - vs, y + hs);
			glVertex2f(osx + vs, y + hs);
			glVertex2f(oldMouseOSX + hs, oldMouseY + hs);
			glVertex2f(oldMouseOSX - hs, oldMouseY + hs);

			glVertex2f(osx - vs, y - hs);
			glVertex2f(osx - vs, y + hs);
			glVertex2f(oldMouseOSX - hs, oldMouseY + hs);
			glVertex2f(oldMouseOSX - hs, oldMouseY - hs);
			osx += 1.0f;
			oldMouseOSX += 1.0f;
		}
		glEnd();*/

		drawMouseOffScreen();

		osx -= 2.0f;
		oldMouseOSX -= 2.0f;
		Fbo::unbind();
	}
	if (game->isKeyDown('g'))
	{
		flower.bindMat();
		glColor3f(0.0f, 0.0f, 0.0f);
		glDisable(GL_BLEND);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(1.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(0.0f, 1.0f);
		glEnd();

		Fbo::unbind();
	}
	if (!paused)
		flower.iterate(iterations, this);

	if (streamlining)
	{
		flower.bindVel();
		float sx = float(game->getMouseX()) / float(game->getResY());
		float sy = 1.0f - float(game->getMouseY()) / float(game->getResY());
		float x = (sx-0.5* float(game->getResX())/ float(game->getResY()))/scale+ viewX;
		float y = (sy-0.5)/scale+ viewY;
		if(x>0)
			x=x-static_cast<int>(x);
		else
			x=x-static_cast<int>(x)+1.0f;
		if(y>0)
			y=y-static_cast<int>(y);
		else
			y=y-static_cast<int>(y)+1.0f;

		slx[0] = x;
		sly[0] = y;
		while (slx[0] < 0.0f)
			slx[0] += 1.0f;
		while (sly[0] < 0.0f)
			sly[0] += 1.0f;
		while (slx[0] >= 1.0f)
			slx[0] -= 1.0f;
		while (sly[0] >= 1.0f)
			sly[0] -= 1.0f;
		for (int i = 1; i < slpnum; i++)
		{
			float pixel[3], xv, yv;
			float px = slx[i - 1], py = sly[i - 1];
			while (px < 0.0f)
				px += 1.0f;
			while (py < 0.0f)
				py += 1.0f;
			while (px >= 1.0f)
				px -= 1.0f;
			while (py >= 1.0f)
				py -= 1.0f;
			//glReadPixels(int(px*float(size)),int(py*float(size)),1,1,GL_RGB,GL_FLOAT,(void *)pixel);
			getSpeed(size, pixel, px, py);
			xv = pixel[0];
			yv = pixel[1];
			float v = sqrt(xv * xv + yv * yv);
			if (v == 0.0f)
				v = 1.0f;
			float dl = 0.005f/scale;

			getSpeed(size, pixel, px + dl * xv / v / 2.0f, py + dl * yv / v
					/ 2.0f);
			xv = pixel[0];
			yv = pixel[1];
			v = sqrt(xv * xv + yv * yv);
			if (v == 0.0f)
				v = 1.0f;

			slx[i] = slx[i - 1] + dl * xv / v;
			sly[i] = sly[i - 1] + dl * yv / v;

		}
		Fbo::unbind();
	}

	if (game->isRightDown() && !painting)
	{
		flower.bindInk();
		Shader::disable();
		glActiveTextureARB(GL_TEXTURE2_ARB);
		glDisable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE0_ARB);
		glDisable(GL_TEXTURE_2D);

		glColor3f(inkRed*(1.0f-inkNew)+inkRedNew*inkNew,inkGreen*(1.0f-inkNew)+inkGreenNew*inkNew,inkBlue*(1.0f-inkNew)+inkBlueNew*inkNew);
		//	glColor3f(1.0,1.0,1.0);
		drawMouseOffScreen();
		Fbo::unbind();
	}
	if (game->isKeyDown('t'))
	{
		if(!coordInk)
		{
			flower.bindInk();
			Shader::disable();
			glActiveTextureARB(GL_TEXTURE2_ARB);
			glDisable(GL_TEXTURE_2D);
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glDisable(GL_TEXTURE_2D);
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glDisable(GL_TEXTURE_2D);

			glColor3f(0.0f, 0.0f, 0.0f);
			glBegin(GL_QUADS);
			glVertex2f(0.0f, 0.0f);
			glVertex2f(1.0f, 0.0f);
			glVertex2f(1.0f, 1.0f);
			glVertex2f(0.0f, 1.0f);
			glEnd();
			Fbo::unbind();
		}
		else
		{
			TexGroup::getSingleton()->bind(flowerTex, 0);
			initFromFGMat.enable();
			flower.bindInk();

			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(1.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(0.0f, 1.0f);
			glEnd();
			Fbo::unbind();
			Shader::disable();
		}
	}
	if (game->isKeyDown('r'))
	{
		flower.bindVel();
		Shader::disable();
		glActiveTextureARB(GL_TEXTURE2_ARB);
		glDisable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE0_ARB);
		glDisable(GL_TEXTURE_2D);

		glColor3f(0.0f, 0.0f, 0.0f);
		glBegin(GL_QUADS);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(1.0f, 0.0f);
		glVertex2f(1.0f, 1.0f);
		glVertex2f(0.0f, 1.0f);
		glEnd();
		Fbo::unbind();
	}
	displayResult();


	float sx = float(game->getMouseX()) / float(game->getResY());
	float sy = 1.0f - float(game->getMouseY()) / float(game->getResY());
	x = (sx-0.5* float(game->getResX())/ float(game->getResY()))/scale+ viewX;
	y = (sy-0.5)/scale+ viewY;
	if(x>0)
		x=x-static_cast<int>(x);
	else
		x=x-static_cast<int>(x)+1.0f;
	if(y>0)
		y=y-static_cast<int>(y);
	else
		y=y-static_cast<int>(y)+1.0f;
	float s = 0.02/scale;
	oldx1=x - s;
	oldx2=x + s;
	oldy1=y - s;
	oldy2=y + s;
}
void FlowShower::keyPress(char key)
{
	if (key == 27)
		game->UIEvent("exit");
	else if (key == 's')
		showSides = !showSides;
	else if (key == ' ')
		painting = !painting;
	else if (key == 'a')
		streamlining = !streamlining;
	else if (key == 'c')
		coordInk = !coordInk;
	else if (key == 'b')
	{

		/*	pngRawInfo info;
		 pngLoadRaw("data/textures/bg.png", &info);
		 //	int width = info.Width;
		// int height = info.Height;*
		 Texgroup::getSingleton()->bind(inkField[currentInkField]);
		 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F_ARB, inksize, inksize, 0,
		 GL_RGBA, GL_UNSIGNED_BYTE, info.Data);*/
	}
	else if (key == 'p')
		showRho = !showRho;
	else if (key == 'm')
		showMat = !showMat;
	else if (key == 'o')
		showRot = !showRot;
	else if (key == 'i')
		showDiv = !showDiv;
	else if (key == '+')
		iterations++;
	else if (key == '-')
	{
		if (iterations)
			iterations--;
	}
	else if (key == 'q')
		paused = !paused;
}
void FlowShower::mouseClick(int button, int state)
{
#if !defined(GLUT_WHEEL_UP)
#  define GLUT_WHEEL_UP   3
#  define GLUT_WHEEL_DOWN 4
#endif

	if (state == GLUT_UP)
	{
		if (button == GLUT_WHEEL_UP)
		{
			scaleb *= 1.2f;
		}
		else if (button == GLUT_WHEEL_DOWN)
		{
			scaleb /= 1.2f;
		}

	}
	else if (state == GLUT_DOWN)
	{
		if (button == GLUT_MIDDLE_BUTTON)
		{
			pressX = float(game->getMouseX()) / float(game->getResY());
			pressY = float(game->getMouseY()) / float(game->getResY());
			pressViewX = viewX;
			pressViewY = viewY;
		}

	}
}
void FlowShower::mouseMove(int x, int y)
{
}
void FlowShower::update()
{
	scale += (scaleb - scale) * 0.1;
	if (game->isMiddleDown() && !painting)
	{
		viewX = pressViewX + (pressX - float(game->getMouseX())
				/ float(game->getResY())) / scale;
		viewY = pressViewY - (pressY - float(game->getMouseY())
				/ float(game->getResY())) / scale;
	}
	inkNew+=0.1f;
	while(inkNew>=1.0f)
	{
		inkNew-=1.0f;
		inkRed=inkRedNew;
		inkGreen=inkGreenNew;
		inkBlue=inkBlueNew;
		inkRedNew=randf(1.0f);
		inkGreenNew=randf(1.0f);
		inkBlueNew=randf(1.0f);
	}
	clock_t curentClock = clock();
	/*if (lastClock != 0 || dt != 0)
	 dt = float(curentClock - lastClock) / 1000.0f;*/
	lastClock = curentClock;

}
