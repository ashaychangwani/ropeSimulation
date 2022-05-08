/* SIMPLE MATHS LIBRARY
PREPARED BY GIZEM KAYAR FOR COMPUTER GRAPHICS COURSE/NEW YORK UNIVERSITY
YEAR: 2022
*/

#pragma once
#include "math3d.h"
using namespace std;
#include <vector>

enum IntegrationScheme
{
	Euler,
	EulerCromer,
	Verlet
};

class Particle
{
private:
	vector3f force;
	vector3f velocity;
	vector3f position;
	vector3f oldPosition;
	vector3f color; //RGB
	bool isFixed;

public:
	Particle()
	{
		clear();
		isFixed = false;
	}

	Particle(vector3f f, vector3f v, vector3f p)
	{
		force = f;
		velocity = v;
		position = p;
		isFixed = false;
	}

	//Setters and getters
	vector3f getForce() { return force; }
	vector3f getVelocity() { return velocity; }
	vector3f getPosition() { return position; }
	vector3f getOldPosition() { return oldPosition; }
	vector3f getColor() { return color; }
	bool getFixed() { return isFixed; }


	void fix() { 
		isFixed = true; 
		setColor(vector3f(0, 1, 0));
		this->clearForce();
		this->clearVelocity();
	}
	void unfix() { 
		isFixed = false;
		setColor(vector3f(1, 0, 0));
		this->setOldPosition(this->getPosition());
		this->clearForce();
		this->clearVelocity();
	}
	void setForce(vector3f f) { force = f; }
	void setVelocity(vector3f v) { velocity = v; }
	void setPosition(vector3f p) {
		position = p;
	}
	void setOldPosition(vector3f p) { oldPosition = p; }
	void setColor(vector3f c) { color = c; }

	//Important functions
	void clear()
	{
		force.clear();
		velocity.clear();
		position.clear();
	}
	void clearForce() { force.clear(); }
	void clearVelocity() { velocity.clear(); }
	void clearPosition() { position.clear(); }
	void applyForce(vector3f f) { this->force += f; }
};

class Spring
{
public:
	int p1, p2;
	float initLength;
};

class Triangle
{
public:
	int p1, p2, p3;
	vector3f normal;
};

class CubeMesh {
public:
	vector<Particle> particles;
	vector<Spring> springs;

	vector3f position;
	vector3f initialPos;
	vector3f velocity;


};