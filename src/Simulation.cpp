#include "Simulation.h"
#define SIGN(x) (x >= 0 ? 1.0 : -1.0)
#include <stdlib.h>
#include "math3d.h"


void Simulation::initializeMesh()
{
	srand(0);

	float cornerx = -8, cornery = -1, cornerz = 4;
	mesh.particles.clear();
	mesh.springs.clear();

	for (int i = 0; i < this->numParticles; i++)
	{
		Particle  particle;
		particle.clearForce();
		particle.setPosition(vector3f(cornerx + this->distance * i, cornery, cornerz));
		particle.setColor(vector3f(1, 0, 0));

		particle.setOldPosition(vector3f(cornerx + this->distance * i, cornery, cornerz));
		mesh.particles.push_back(particle);
	}
	mesh.particles[0].fix();
	Spring s;
	// each pair shows the id of the particles in the cube for each spring
	int left = 0;
	int right = 1;
	for (int i = 0; i < this->numParticles-1; i ++)
	{
		Spring s;
		s.p1 = left++;
		s.p2 = right++;
		vector3f pos1 = mesh.particles[s.p1].getPosition();
		vector3f pos2 = mesh.particles[s.p2].getPosition();
		s.initLength = (pos1 - pos2).length();
		mesh.springs.push_back(s);
	}
}



void Simulation::simulateEuler(Particle* p)
{
	p->setPosition(p->getPosition() + p->getVelocity() * this->timestep);
	p->setVelocity(p->getVelocity() + p->getForce() * this->timestep / this->commonMass);
}

void Simulation::simulateEulerCromer(Particle* p)
{
	p->setVelocity(p->getVelocity() + p->getForce() * this->timestep / this->commonMass);
	p->setPosition(p->getPosition() + p->getVelocity() * this->timestep);
}

void Simulation::simulateVerlet(Particle* p)
{
	vector3f posTemp = p->getPosition();
	if (!p->getFixed()) {
		p->setPosition((p->getPosition() * 2) - p->getOldPosition() + (p->getForce() / this->commonMass) * (this->timestep * this->timestep));
		p->setOldPosition(posTemp);
		p->setVelocity((p->getPosition() - p->getOldPosition()) / this->timestep);
	}
}

float Simulation::distanceCalc(Particle* p1, Particle* p2) {
	vector3f p1Position = p1->getPosition();
	vector3f p2Position = p2->getPosition();
	float dx = p1Position.getX() - p2Position.getX();
	float dy = p1Position.getY() - p2Position.getY();
	float dz = p1Position.getZ() - p2Position.getZ();
	float res = sqrtf(dx * dx + dy * dy + dz * dz);
	return res;
}

void Simulation::relaxConstraints(Particle* p1, Particle* p2) {
	vector3f direction = p2->getPosition() - p1->getPosition();
	direction.makeUnit();
	float delta = distanceCalc(p1, p2) - this->distance;
	vector3f offset = direction * (delta / 2);
	vector3f p1Position = p1->getPosition();
	vector3f p2Position = p2->getPosition();
	bool p1Fixed = p1->getFixed();
	bool p2Fixed = p2->getFixed();
	if (p1Fixed && p2Fixed)
		return;
	if (p1Fixed) {
		p2->setPosition(p2Position - offset*2);
	}
	else if (p2Fixed) {
		p1->setPosition(p1Position + offset*2);
	}
	else {
		p1->setPosition(p1Position + offset);
		p2->setPosition(p2Position - offset);
	}
}


void Simulation::clearForces()
{
	for (int i = 0; i < mesh.particles.size(); i++)
		mesh.particles[i].clearForce();
}

void Simulation::destroyParticles()
{
	mesh.particles.clear();
}

void Simulation::solveWorldCollision()
{
	vector3f tempVel;
	vector3f tempPos;

	for (int i = 0; i < mesh.particles.size(); i++)
	{
		tempVel = mesh.particles[i].getVelocity();
		tempPos = mesh.particles[i].getPosition();

		if (mesh.particles[i].getPosition().getX() <= -halfWorld.getX() || mesh.particles[i].getPosition().getX() >= halfWorld.getX())
		{
			tempVel.setX(tempVel.getX() * -worldColldConst);
			tempPos.setX(SIGN(tempPos.getX()) * halfWorld.getX());
		}

		if (mesh.particles[i].getPosition().getY() <= -halfWorld.getY() || mesh.particles[i].getPosition().getY() >= halfWorld.getY())
		{
			tempVel.setY(tempVel.getY() * -worldColldConst);
			tempPos.setY(SIGN(tempPos.getY()) * halfWorld.getY());
		}

		if (mesh.particles[i].getPosition().getZ() <= -halfWorld.getZ() || mesh.particles[i].getPosition().getZ() >= halfWorld.getZ())
		{
			tempVel.setZ(tempVel.getZ() * -worldColldConst);
			tempPos.setZ(SIGN(tempPos.getZ()) * halfWorld.getZ());
		}

		mesh.particles[i].setVelocity(tempVel);
		mesh.particles[i].setPosition(tempPos);
	}
}

void Simulation::computeSystemEnergies()
{
	kinEn = 0.0f;
	potEn = 0.0f;
	sprEn = 0.0f;

	//TO DO - Compute kinetic and potential energy

	for (int i = 0; i < mesh.particles.size(); i++)
	{
		Particle p = mesh.particles[i];
		this->kinEn += (this->commonMass * p.getVelocity().lengthSquare()) / 2;

		this->potEn += (9.81f * (this->commonMass) * (p.getPosition().getY() + this->halfWorld.getY()));
	}

	//TO DO - Compute spring energy
	for (int s = 0; s < mesh.springs.size(); s++)
	{
		vector3f p1Position = mesh.particles[mesh.springs[s].p1].getPosition();
		vector3f p2Position = mesh.particles[mesh.springs[s].p2].getPosition();
		float distance = sqrtf(pow(p1Position.getX() - p2Position.getX(), 2) + pow(p1Position.getY() - p2Position.getY(), 2) + pow(p1Position.getZ() - p2Position.getZ(), 2)) - mesh.springs[s].initLength;
		this->sprEn += (spring_constant * distance * distance) / 2;
	}


}
