#pragma once
#include "Actor.h"
#include <string>

class EnergyLauncher : public Actor
{
public:
	EnergyLauncher(class Game* game, float inCooldown, std::string inDoor);
	~EnergyLauncher();
	void OnUpdate(float deltaTime) override;

private:
	static float constexpr mCollisionSideLen = 50.0f;
	static float constexpr mForwardSpawnDist = 20.0f;
	float mCooldown;
	float mCooldownTimer;
	std::string mDoor;
};