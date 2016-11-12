#include "Particle.h"

ParticleObject::ParticleObject(ParticleObject_TYPE typeValue) :
type(typeValue),
pos(0, 0, 0),
scale(1,1,1),
rotation(0),
rotationSpeed(0),
active(false),
bounce(false),
m_lifetime(0.2)
{}

ParticleObject::~ParticleObject()
{}

