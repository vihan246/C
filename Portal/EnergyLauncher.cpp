#include "EnergyLauncher.h"
#include "Game.h"
#include "CollisionComponent.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "Pellet.h"
#include "Door.h"

EnergyLauncher::EnergyLauncher(Game* game, float inCooldown, std::string inDoor = "")
: Actor(game)
{
	MeshComponent* mc = new MeshComponent(this);
	mc->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshes/EnergyLauncher.gpmesh"));
	CollisionComponent* cc = new CollisionComponent(this);
	cc->SetSize(mCollisionSideLen, mCollisionSideLen, mCollisionSideLen);
	mCooldown = inCooldown;
	mCooldownTimer = 0.0f;
	mDoor = inDoor;
	game->AddCollider(this);
}

EnergyLauncher::~EnergyLauncher()
{
	mGame->RemoveCollider(this);
	mGame->RemoveActor(this);
}

void EnergyLauncher::OnUpdate(float deltaTime)
{
	mCooldownTimer += deltaTime;
	if (mCooldownTimer >= mCooldown && (mDoor.empty() || !mGame->GetDoor(mDoor)->IsOpen()))
	{
		Pellet* pellet = new Pellet(mGame);
		GetGame()->GetAudio()->PlaySound("PelletFire.ogg", false, this);
		pellet->SetPosition((mPosition + (GetForward() * mForwardSpawnDist)));
		pellet->CalcWorldTransform();
		pellet->SetDirection(GetForward());
		mCooldownTimer = 0.0f;
	}
}