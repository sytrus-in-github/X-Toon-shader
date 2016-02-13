#pragma once
#include "Vec3.h"

class Light
{
public:
	Light(Vec3f clr, Vec3f pos);
	~Light();
	Vec3f color;
	Vec3f position;
};

