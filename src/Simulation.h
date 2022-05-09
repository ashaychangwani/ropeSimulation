/* SIMPLE MATHS LIBRARY
PREPARED BY GIZEM KAYAR FOR COMPUTER GRAPHICS COURSE/NEW YORK UNIVERSITY
YEAR: 2022
*/

#pragma once
#include <math.h>
#include "particle.h"

using namespace std;



class Simulation
{
public:
	float timestep;
	float distance;
	CubeMesh mesh;
	int numParticles;
	int numRelax;
	float commonMass;
	float kinEn;
	float potEn;
	float sprEn;
	IntegrationScheme is;

	float spring_constant;
	float damping_constant;

	float worldColldConst;
	vector3f world;
	vector3f halfWorld;
	vector3f gravity;

	Simulation()
	{
		commonMass = 1.0f;
		timestep = 0.0005f;
		distance = 0.035f;
		numParticles = 500;
		numRelax = 10;

		world.setUp(20, 20, 20);
		halfWorld = world * 0.5f;
		gravity = vector3f(0.0f, -9.81f, 0.0f);
		initializeMesh();

		worldColldConst = 0.5f;

		//TO DO - Try different values and integration schemes
		is = Verlet;

		spring_constant = 1000;
		damping_constant = 1;
	}

	void initializeMesh();
	virtual void simulateEuler(Particle* p);
	virtual void simulateEulerCromer(Particle* p);
	virtual void simulateVerlet(Particle* p);
	virtual void relaxConstraints(Particle* p1, Particle* p2);
	virtual float distanceCalc(Particle* p1, Particle* p2);

	virtual void reverseGravity() {
		gravity.setX(gravity.getX() * -1);
		gravity.setY(gravity.getY() * -1);
		gravity.setZ(gravity.getZ() * -1);
	}

	virtual void applyGravity(int dir) {
		if (dir == 1)
			gravity.setX(9.81f);
		else if (dir == 2)
			gravity.setX(-9.81f);
		else
			gravity.setX(0.0f);
	}


	virtual void changeAnchors() {
		for (int i = 0; i < this->numParticles; i++) {
			mesh.particles[i].unfix();
		}
		mesh.particles[rand() % this->numParticles].fix();
		mesh.particles[rand() % this->numParticles].fix();
	}
	virtual void simulate()
	{
		clearForces();
		applyForces();


		solveWorldCollision();

		for (int j = 0; j < mesh.particles.size(); j++)
		{
			switch (is)
			{
			case Euler:
				simulateEuler(&mesh.particles[j]);
				break;
			case EulerCromer:
				simulateEulerCromer(&mesh.particles[j]);
				break;
			case Verlet:
				simulateVerlet(&mesh.particles[j]);
				break;

			}
		}
		for (int i = 0; i < this->numRelax; i++) {
			for (int s = 0; s < mesh.springs.size(); s++) {
				relaxConstraints(&mesh.particles[mesh.springs[s].p1], &mesh.particles[mesh.springs[s].p2]);
			}
		}
		computeSystemEnergies();

	}

	
	void clearForces();
	virtual void applyForces() {};
	void destroyParticles();
	void solveWorldCollision();
	void computeSystemEnergies();

};

//Spring Mesh movement
class SimSpring : public Simulation
{
public:

	SimSpring() : Simulation()
	{

	}

	virtual void applyForces()
	{
		
		//TO DO - Apply gravity
		for (int i = 0; i < mesh.particles.size(); i++)
		{
			if(!mesh.particles[i].getFixed())
				mesh.particles[i].applyForce(this->gravity * this->commonMass);
		}

		//TO DO - Apply spring forces
		/*for (int i = 0; i < mesh.springs.size(); i++)
		{
			Particle& p1 = mesh.particles[mesh.springs[i].p1];
			Particle& p2 = mesh.particles[mesh.springs[i].p2];

			//Add spring Force
			vector3f dist = p2.getPosition() - p1.getPosition();
			vector3f force((dist / dist.length()) * spring_constant * (dist.length() - mesh.springs[i].initLength));

			p1.applyForce(force);
			p2.applyForce(force * -1);
			
			/*vector3f p1Position = p1.getPosition();
			vector3f p2Position = p2.getPosition();
			float distance = sqrtf(pow(p1Position.getX() - p2Position.getX(), 2) + pow(p1Position.getY() - p2Position.getY(), 2) + pow(p1Position.getZ() - p2Position.getZ(), 2));
			vector3f vec((p1Position.getX() - p2Position.getX()) / distance, (p1Position.getY() - p2Position.getY()) / distance, (p1Position.getZ() - p2Position.getZ()) / distance);

			if (distance > mesh.springs[i].initLength) {
				p1.applyForce(vec * -1 * spring_constant * distance);
				p2.applyForce(vec * spring_constant * distance);
			}
			else {
				p2.applyForce(vec * -1 * spring_constant * distance);
				p1.applyForce(vec * spring_constant * distance);
			}

			//Add damping Force
			vector3f dt = p2.getPosition() - p1.getPosition();
			dt = dt / dt.length();
			vector3f mt = dt * p2.getVelocity().length() - dt * p1.getVelocity().length();
			
			p1.applyForce(dt * mt.length() * damping_constant);
			p2.applyForce(dt * mt.length() * damping_constant * -1);
			

			/*if (distance > mesh.springs[i].initLength) {
				p2.applyForce(p2.getVelocity() * damping_constant);
				p1.applyForce(p1.getVelocity() * -1 * damping_constant);
			}
			else {
				p1.applyForce(p1.getVelocity() * damping_constant);
				p2.applyForce(p2.getVelocity() * -1 * damping_constant);
			}
		}*/
	}
};
