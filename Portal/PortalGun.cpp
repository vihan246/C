#include "PortalGun.h"
#include "MeshComponent.h"
#include "Game.h"
#include "CollisionComponent.h"
#include "Renderer.h"
#include "Player.h"

PortalGun::PortalGun(Game* game)
: Actor(game)
{
	MeshComponent* mc = new MeshComponent(this);
	mc->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshes/PortalGun.gpmesh"));
	CollisionComponent* cc = new CollisionComponent(this);
	cc->SetSize(mCollisionSideSize, mCollisionSideSize, mCollisionSideSize);
}

void PortalGun::OnUpdate(float deltaTime)
{
	mRotation += Math::Pi * deltaTime;
	Player* player = GetGame()->GetPlayer();

	if (GetComponent<CollisionComponent>()->Intersect(player->GetComponent<CollisionComponent>()))
	{
		SetState(ActorState::Destroy);
		player->GivePlayerGun();
	}
}