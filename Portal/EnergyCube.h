#pragma once
#include "Actor.h"

class EnergyCube : public Actor
{
public:
	EnergyCube(class Game* game);
	~EnergyCube();

private:
	static float constexpr mCollisionSideLen = 25.0f;
};