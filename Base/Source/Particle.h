#ifndef _PARTICLE_H
#define _PARTICLE_H

#include "Vector3.h"

enum ParticleObject_TYPE
{
	P_SMOKE,
	P_TOTAL
};

class ParticleObject
{
public:
	ParticleObject(ParticleObject_TYPE = P_SMOKE);
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