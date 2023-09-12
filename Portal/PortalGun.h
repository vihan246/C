#pragma once
#include "Actor.h"

class PortalGun : public Actor
{
public:
	PortalGun(class Game* game);
	void OnUpdate(float deltaTime) override;

private:
	static constexpr float mCollisionSideSize = 8.0f;
};