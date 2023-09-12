#include "EnergyGlass.h"
#include "Game.h"
#include "Renderer.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"

EnergyGlass::EnergyGlass(Game* game)
: Actor(game)
{
	MeshComponent* mc = new MeshComponent(this, true);
	mc->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshes/Cube.gpmesh"));
	mc->SetTextureIndex(17);

	CollisionComponent* cc = new CollisionComponent(this);
	cc->SetSize(mCollisionSideLen, mCollisionSideLen, mCollisionSideLen);

	game->AddCollider(this);
}

EnergyGlass::~EnergyGlass()
{
	mGame->RemoveCollider(this);
}