#include "TurretBase.h"
#include "Game.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "Renderer.h"
#include "TurretHead.h"
#include "HealthComponent.h"

TurretBase::TurretBase(Game* game)
: Actor(game)
{
	SetScale(0.75f);
	MeshComponent* mc = new MeshComponent(this);
	mc->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshes/TurretBase.gpmesh"));
	CollisionComponent* cc = new CollisionComponent(this);
	cc->SetSize(25.0f, 110.0f, 25.0f);

	mHead = new TurretHead(game, this);
	HealthComponent* hc = new HealthComponent(this);
	hc->SetOnDeath([this] {
		Die();
	});
	hc->SetOnDamage([this](const Vector3&) {
		mHead->TakeDamage();
	});

	game->AddCollider(this);
}

TurretBase::~TurretBase()
{
	mGame->RemoveCollider(this);
}

void TurretBase::Die()
{
	mHead->Die();
}