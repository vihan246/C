#pragma once
#include "Actor.h"

class TurretBase : public Actor
{
public:
	TurretBase(class Game* game);
	~TurretBase();
	void Die();

private:
	class TurretHead* mHead;
};