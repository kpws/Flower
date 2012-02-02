/*
 * Flower.h
 *
 *  Created on: Feb 10, 2010
 *      Author: petter
 */

#ifndef FLOWER_H_
#define FLOWER_H_

#include "Shader.h"
#include "Fbo.h"
#include "Obstacligizer.h"

class Flower
{
public:
	Flower();
	virtual ~Flower();

	void load(int size,int inksize);
	void iterate(int n,Obstacligizer* ogizer);
	void sendObstacleInfo(float px,float py,float vx,float vy,float omega);
	int getFlowTex();
	int getInkTex();
	int getMatTex();
	void bindInk();
	void bindMat();
	void bindVel();
private:
	int size,inksize;
	float dt;
	Shader updateMat,initMat,moveInkMat,obstacleMat;
	int flowField[2],matField,inkField[2];
	Fbo flowFieldFbo[2],matFieldFbo,inkFieldFbo[2];
	int currentField,currentInkField;
	GLint obsSpeedLocation,obsRotLocation,obsPosLocation,moveInkMatIterLoc;
};

#endif /* FLOWER_H_ */
