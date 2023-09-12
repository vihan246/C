#pragma once
#include "Actor.h"
class Block : public Actor
{
public:
	Block(class Game* game);
	~Block();

private:
	static constexpr float mBlockScale = 64.0f;
	static constexpr float mCollisionSize = 1.0f;
};