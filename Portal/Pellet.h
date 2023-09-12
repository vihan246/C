#pragma once
#include "Actor.h"

class Pellet : public Actor
{
public:
	Pellet(class Game* game);
	void SetDirection(Vector3 inDir) { mMovementDir = inDir; };
	void OnUpdate(float deltaTime) override;

private:
	static float constexpr mCollisionSideLen = 25.0f;
	Vector3 mMovementDir = Vector3::Zero;
	float mMovementSpeed = 500.0f;
	float mInvincibleTimer = 0.25f;
	static float constexpr mInvulnerableTime = 0.25f;
	bool mGreen = false;
	static float constexpr mPelletDamage = 100.0f;
};