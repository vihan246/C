#include "EnergyCube.h"
#include "Game.h"
#include "Renderer.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"

EnergyCube::EnergyCube(Game* game)
: Actor(game)
{
	MeshComponent* mc = new MeshComponent(this);
	mc->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshes/EnergyCube.gpmesh"));

	CollisionComponent* cc = new CollisionComponent(this);
	cc->SetSize(mCollisionSideLen, mCollisionSideLen, mCollisionSideLen);

	game->AddCollider(this);
}

EnergyCube::~EnergyCube()
{
	mGame->RemoveCollider(this);
}