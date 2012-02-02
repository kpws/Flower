/*
 * Flower.cpp
 *
 *  Created on: Feb 10, 2010
 *      Author: petter
 */

#include "Flower.h"

Flower::Flower()
{
	// TODO Auto-generated constructor stub

}

Flower::~Flower()
{
	// TODO Auto-generated destructor stub
}

void Flower::load(int size,int inksize)
{
	this->size=size;
	this->inksize=inksize;
	initMat.load("init.frag", "orth.vert");

	updateMat.load("update.frag", "orth.vert");
	updateMat.enable();
	updateMat.sendInt(updateMat.getLocation("res"), size);
	updateMat.sendInt(updateMat.getLocation("tex"), 0);
	updateMat.sendInt(updateMat.getLocation("mattex"), 1);
	updateMat.sendInt(updateMat.getLocation("inktex"), 2);
	updateMat.disable();

	obstacleMat.load("obstacle.frag", "orth_texgen.vert");
	obstacleMat.enable();
	obsSpeedLocation = obstacleMat.getLocation("speed");
	obsRotLocation = obstacleMat.getLocation("rotspeed");
	obsPosLocation = obstacleMat.getLocation("midpos");
	obstacleMat.disable();

	moveInkMat.load("moveInk.frag", "orth.vert");
	moveInkMat.enable();
	moveInkMat.sendInt(moveInkMat.getLocation("res"), inksize);
	moveInkMat.sendInt(moveInkMat.getLocation("flowTex"), 0);
	moveInkMat.sendInt(moveInkMat.getLocation("oldInkTex"), 1);
	moveInkMatIterLoc=moveInkMat.getLocation("iterations");
	moveInkMat.disable();

	flowField[0] = TexGroup::getSingleton()->add(GL_RGBA16F_ARB, size, size);
	flowField[1] = TexGroup::getSingleton()->add(GL_RGBA16F_ARB, size, size);
	flowFieldFbo[0].create(size, size);
	flowFieldFbo[1].create(size, size);
	flowFieldFbo[0].attachTex(flowField[0], 0);
	flowFieldFbo[1].attachTex(flowField[1], 0);

	inkField[0]= TexGroup::getSingleton()->add(GL_RGB32F_ARB, inksize, inksize);
	inkField[1]= TexGroup::getSingleton()->add(GL_RGB32F_ARB, inksize, inksize);

	inkFieldFbo[0].create(inksize, inksize);
	inkFieldFbo[1].create(inksize, inksize);
	inkFieldFbo[0].attachTex(inkField[0], 0);
	inkFieldFbo[1].attachTex(inkField[1], 0);

	matField = TexGroup::getSingleton()->add(GL_RGB, size, size);
	matFieldFbo.create(size, size);
	matFieldFbo.attachTex(matField, 0);

	currentField = 0;
	currentInkField = 0;

	dt = 0.002;
}
void Flower::iterate(int n,Obstacligizer* ogizer)
{
	for(int i=0;i<n;i++)
	{
		currentField = (currentField + 1) % 2;

		TexGroup::getSingleton()->bind(flowField[(currentField + 1) % 2], 0);//use the old field
		TexGroup::getSingleton()->bind(matField, 1);
		TexGroup::getSingleton()->bind(inkField[currentInkField], 2);
		flowFieldFbo[currentField].bind();//make the new field

		updateMat.enable();
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

		obstacleMat.enable();
		ogizer->obstacligize(dt);

		/*if (game->isKeyDown('r'))
		{
			initMat.enable();
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
		}*/
		Fbo::unbind();
	}


	currentInkField = (currentInkField + 1) % 2;
	TexGroup::getSingleton()->bind(flowField[currentField], 0);
	TexGroup::getSingleton()->bind(inkField[(currentInkField + 1) % 2], 1);
	inkFieldFbo[currentInkField].bind();
	moveInkMat.enable();
	moveInkMat.sendInt(moveInkMatIterLoc,n);
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
	inkFieldFbo[currentField].unbind();
}
void Flower::sendObstacleInfo(float px,float py,float vx,float vy,float omega)
{
	obstacleMat.send2Floats(obsSpeedLocation,vx,vy );
		obstacleMat.send2Floats(obsRotLocation,0.0f,0.0f);
}
int Flower::getFlowTex()
{
	return flowField[currentField];
}
int Flower::getInkTex()
{
	return inkField[currentInkField];
}
int Flower::getMatTex()
{
	return matField;
}
void Flower::bindInk()
{
	inkFieldFbo[currentInkField].bind();
}
void Flower::bindMat()
{
	matFieldFbo.bind();
}
void Flower::bindVel()
{
	flowFieldFbo[currentField].bind();
}
