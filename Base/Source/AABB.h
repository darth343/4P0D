#ifndef _COLLIDER_AABB_H
#define _COLLIDER_AABB_H

#include "Vector3.h"

struct AABB
{
    AABB()
    {}
    ~AABB()
    {}

    void Update(Vector3 pos, Vector3 scale)
    {
        m_MaxPoint = pos + scale * 0.5;
        m_MinPoint = pos - scale * 0.5;
    }

    Vector3 m_MaxPoint;
    Vector3 m_MinPoint;
};

#endif