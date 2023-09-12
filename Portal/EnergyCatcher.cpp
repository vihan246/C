#include "EnergyCatcher.h"
#include "Game.h"
#include "CollisionComponent.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "Door.h"

EnergyCatcher::EnergyCatcher(Game* game, std::string inDoor = "")
: Actor(game)
{
	mDoor = inDoor;
	MeshComponent* mc = new MeshComponent(this);
	mc->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshes/EnergyCatcher.gpmesh"));

	CollisionComponent* cc = new CollisionComponent(this);
	cc->SetSize(mCollisionSideLen, mCollisionSideLen, mCollisionSideLen);

	game->AddCollider(this);
}

EnergyCatcher::~EnergyCatcher()
{
	mGame->RemoveCollider(this);
}

void EnergyCatcher::Activate()
{
	if (!mActivated)
	{
		mActivated = true;
		GetGame()->GetAudio()->PlaySound("EnergyCaught.ogg", false, this);
		if (!mDoor.empty())
		{
			mGame->GetDoor(mDoor)->Open();
		}
	}
}