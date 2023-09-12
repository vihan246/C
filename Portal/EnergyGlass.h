#pragma once
#include "Actor.h"

class EnergyGlass : public Actor
{
public:
	EnergyGlass(class Game* game);
	~EnergyGlass();

private:
	static float constexpr mCollisionSideLen = 1.0f;
};
