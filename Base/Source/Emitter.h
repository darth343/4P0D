#ifndef _EMITTER_H
#define _EMITTER_H

#include "Particle.h"
#include <vector>

class Emitter
{
public:
    Emitter() 
    {
        m_ParticleCount = 0;
    }
    ~Emitter()
    {
    }

	void SetEmitterPosition(Vector3 pos)
	{
		m_Pos = pos;
	}

	Vector3 GetEmitterPosition()
	{
		return m_Pos;
	}

    void SetSpawnLocation(Vector3 MinPos, Vector3 MaxPos)
    {
        m_SpawnMinPos = MinPos;
        m_SpawnMaxPos = MaxPos;
    }

    void SpawnParticle()
    {
		for (int i = 0; i < m_SpawnRate; ++i)
		{
			if (m_ParticleCount < m_MaxParticles)
			{
				ParticleObject* particle = GetParticle();

				particle->type = m_ParticleToSpawnType;

				switch (particle->type)
				{
				case ParticleObject_TYPE::P_SMOKE:
				{
					particle->scale.Set(20, 20, 20);
					particle->vel.Set(0, 0, 0);
					particle->bounce = true;
					break;
				}

                case ParticleObject_TYPE::P_NORMAL_BLOOD:
                {
                    particle->scale.Set(20, 20, 20);
                    particle->vel = Vector3(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5), 0);
                    particle->bounce = true;
                    break;
                }

                case ParticleObject_TYPE::P_GREEN_BLOOD:
                {
                    particle->scale.Set(20, 20, 20);
                    particle->vel = Vector3(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5), 0);
                    particle->bounce = true;
                    break;
                }

                case ParticleObject_TYPE::P_SILVER_BLOOD:
                {
                    particle->scale.Set(20, 20, 20);
                    particle->vel = Vector3(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5), 0);
                    particle->bounce = true;
                    break;
                }
				}

				particle->rotationSpeed = Math::RandFloatMinMax(20.f, 40.f);
				particle->pos.Set(Math::RandFloatMinMax(m_SpawnMinPos.x, m_SpawnMaxPos.x), Math::RandFloatMinMax(m_SpawnMinPos.y, m_SpawnMaxPos.y), Math::RandFloatMinMax(m_SpawnMinPos.z, m_SpawnMaxPos.z));

			}
		}
    }

    std::vector<ParticleObject*> GetParticles()
    {
        return m_ParticleList;
    }

    void SetParticleType(ParticleObject_TYPE ParticleType)
    {
        m_ParticleToSpawnType = ParticleType;
    }

    void SetMaxParticle(int MaxParticles)
    {
        m_MaxParticles = MaxParticles;
    }

    void SetParticleList(std::vector<ParticleObject*>newList)
    {
        m_ParticleList = newList;
    }

	void SetParticleSpawnRate(int rate)
	{
		m_SpawnRate = rate;
	}

    void ReduceParticleCount(int amt)
    {
        m_ParticleCount -= amt;
    }

    double m_lifetime;

private:
    Vector3 m_Pos;
    Vector3 m_SpawnMaxPos;
    Vector3 m_SpawnMinPos;

    float m_MaxParticles;
	float m_SpawnRate;
    float m_ParticleCount;

    ParticleObject_TYPE m_ParticleToSpawnType;

    std::vector<ParticleObject*>m_ParticleList;

    ParticleObject* GetParticle()
    {
        std::vector<ParticleObject*>::iterator it;
        std::vector<ParticleObject*>::iterator end = m_ParticleList.end();
        for (it = m_ParticleList.begin(); it != end; ++it)
        {
            ParticleObject* particle = (ParticleObject*)*it;

            if (!particle->active)
            {
                particle->active = true;
                ++m_ParticleCount;
                return particle;
            }
        }

        for (unsigned i = 0; i < 10; ++i)
        {
            ParticleObject* particle = new ParticleObject();
            m_ParticleList.push_back(particle);
        }

        //ParticleObject* particle = m_ParticleList.back();
        //particle->active = true;
        //++m_ParticleCount;
        return GetParticle();  
    }
};

#endif