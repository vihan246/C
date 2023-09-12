#pragma once
#include "Actor.h"

class PlayerMesh : public Actor
{
public:
	PlayerMesh(class Game* game);
	void OnUpdate(float deltaTime) override;

private:
	const Vector3 PLAYER_MESH_SCALE = Vector3(1.0f, 2.5f, 2.5f);
	const Vector3 UNPROJECT_INPUT = Vector3(300.0f, -250.0f, 0.4f);
};