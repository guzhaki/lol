//
// Lol Engine
//
// Copyright: (c) 2010-2013 Sam Hocevar <sam@hocevar.net>
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the Do What The Fuck You Want To
//   Public License, Version 2, as published by Sam Hocevar. See
//   http://www.wtfpl.net/ for more details.
//

#pragma once

//
// The GraphicEntity class
// -----------------------
//

#include <lol/math/transform.h>

#include "engine/entity.h"

namespace lol
{

class WorldEntity : public Entity
{
public:
    virtual char const *GetName();

public:
    box3 m_aabb;
    vec3 m_position = vec3::zero;
    vec3 m_velocity = vec3::zero;
    quat m_rotation = quat(1);
    vec3 m_rotation_velocity = vec3::zero;

protected:
    WorldEntity();
    virtual ~WorldEntity();

    virtual void TickGame(float seconds);
    virtual void TickDraw(float seconds, Scene &scene);
};

} /* namespace lol */

