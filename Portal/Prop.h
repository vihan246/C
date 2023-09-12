#pragma once
#include "Actor.h"
#include <string>

class Prop : public Actor
{
public:
	Prop(class Game* game, bool inAlpha, bool inCollision, std::string inMesh);
	~Prop();

private:
};