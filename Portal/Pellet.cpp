#include "Pellet.h"
#include "Game.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "CollisionComponent.h"
#include "Player.h"
#include "Portal.h"
#include "EnergyCatcher.h"
#include "EnergyCube.h"
#include "EnergyGlass.h"
#include "HealthComponent.h"

Pellet::Pellet(Game* game)
: Actor(game)
{
	MeshComponent* mc = new MeshComponent(this, true);
	mc->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshes/Sphere.gpmesh"));
	mc->SetTextureIndex(1);

	CollisionComponent* cc = new CollisionComponent(this);
	cc->SetSize(mCollisionSideLen, mCollisionSideLen, mCollisionSideLen);
}

void Pellet::OnUpdate(float deltaTime)
{
	mInvincibleTimer -= deltaTime;
	mPosition += mMovementDir * mMovementSpeed * deltaTime;
	CollisionComponent* collComp = GetComponent<CollisionComponent>();
	if (collComp->Intersect(mGame->GetPlayer()->GetComponent<CollisionComponent>()))
	{
		SetState(ActorState::Destroy);
		mGame->GetAudio()->PlaySound("PelletDeath.ogg", false, this, false);
		mGame->GetPlayer()->GetComponent<HealthComponent>()->TakeDamage(mPelletDamage, mPosition);
	}

	Portal* blue = mGame->GetBluePortal();
	Portal* orange = mGame->GetOrangePortal();
	Portal* exitPortal = nullptr;
	if ((blue != nullptr) && (orange != nullptr))
	{
		bool intersection = false;
		if (collComp->Intersect(blue->GetComponent<CollisionComponent>()))
		{
			exitPortal = orange;
			intersection = true;
		}
		else if (collComp->Intersect(orange->GetComponent<CollisionComponent>()))
		{
			exitPortal = blue;
			intersection = true;
		}
		if (intersection)
		{
			mMovementDir = exitPortal->GetQuatForward();
			mPosition = exitPortal->GetPosition() + mMovementDir * 10.0f;
			mInvincibleTimer = mInvulnerableTime;
		}
	}

	if (mInvincibleTimer <= 0.0f)
	{
		for (auto collider : mGame->GetColliders())
		{
			if (collComp->Intersect(collider->GetComponent<CollisionComponent>()))
			{
				HealthComponent* collHealthComp = collider->GetComponent<HealthComponent>();
				if (collHealthComp && !collHealthComp->IsDead())
				{
					collHealthComp->TakeDamage(mPelletDamage, mPosition);
					mGame->GetAudio()->PlaySound("PelletDeath.ogg", false, this, false);
					SetState(ActorState::Destroy);
				}
				else if (!collHealthComp)
				{
					if (dynamic_cast<EnergyCatcher*>(collider))
					{
						mMovementSpeed = 0.0f;
						mPosition = collider->GetPosition() + collider->GetForward() * 40.0f;
						dynamic_cast<EnergyCatcher*>(collider)->Activate();
					}
					else if (dynamic_cast<EnergyCube*>(collider))
					{
						GetComponent<MeshComponent>()->SetTextureIndex(2);
						mGreen = true;
					}
					else
					{
						if (!(dynamic_cast<EnergyGlass*>(collider) && mGreen))
						{
							mGame->GetAudio()->PlaySound("PelletDeath.ogg", false, this, false);
							SetState(ActorState::Destroy);
						}
					}
				}
			}
		}
	}
}