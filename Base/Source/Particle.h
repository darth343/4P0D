#ifndef _PARTICLE_H
#define _PARTICLE_H

#include "Vector3.h"

enum ParticleObject_TYPE
{
	P_WATER = 0,
    P_LILY,
    P_SPARK,
	P_PORTAL,
	P_TOTAL
};

class ParticleObject
{
public:
	ParticleObject(ParticleObject_TYPE = P_WATER);
	~ParticleObject();

	ParticleObject_TYPE type;
	Vector3 pos;
	Vector3 vel;
	Vector3 scale;
	float rotation;
	float rotationSpeed;
	bool bounce;

	bool active;

};

#endif