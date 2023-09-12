#include "Block.h"
#include "Game.h"
#include "CollisionComponent.h"
#include "MeshComponent.h"
#include "Renderer.h"

Block::Block(Game* game)
: Actor(game)
{
	CollisionComponent* cc = new CollisionComponent(this);
	MeshComponent* mc = new MeshComponent(this);
	cc->SetSize(mCollisionSize, mCollisionSize, mCollisionSize);
	mc->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Meshes/Cube.gpmesh"));

	GetGame()->AddCollider(this);
}

Block::~Block()
{
	GetGame()->RemoveCollider(this);
}