#pragma once
#include "Actor.h"
#include <string>

class EnergyCatcher : public Actor
{
public:
	EnergyCatcher(class Game* game, std::string inDoor);
	~EnergyCatcher();
	void Activate();

private:
	std::string mDoor;
	static float constexpr mCollisionSideLen = 50.0f;
	bool mActivated = false;
};