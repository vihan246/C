#include "TurretHead.h"
#include "Game.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "LaserComponent.h"
#include "HealthComponent.h"
#include "Random.h"
#include "CollisionComponent.h"
#include "Portal.h"
#include "TurretBase.h"

TurretHead::TurretHead(Game* game, Actor* parent)
: Actor(game, parent)
{
	SetScale(0.75f);
	mPosition = Vector3(0.0f, 0.0f, 18.75f);
	MeshComponent* mc = new MeshComponent(this);
	mc->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshes/TurretHead.gpmesh"));

	mLaser = new Actor(game, this);
	mLaser->SetPosition(Vector3(0, 0, 10));
	mLaserComponent = new LaserComponent(mLaser);
	mLaserComponent->SetIgnoreActor(mParent);
	mPrevQuat = GetQuat();
	mPlayingSound = 0;
}

void TurretHead::OnUpdate(float deltaTime)
{
	//mRotation += Math::TwoPi * deltaTime;
	mStateTimer += deltaTime;
	if (mTeleported)
	{
		mTeleportTimer -= deltaTime;
	}
	switch (mTurretState)
	{
	case TurretState::Idle:
		UpdateIdle(deltaTime);
		break;
	case TurretState::Priming:
		UpdatePriming(deltaTime);
		break;
	case TurretState::Search:
		UpdateSearch(deltaTime);
		break;
	case TurretState::Firing:
		UpdateFiring(deltaTime);
		break;
	case TurretState::Falling:
		UpdateFalling(deltaTime);
		break;
	case TurretState::Dead:
		UpdateDead(deltaTime);
		break;
	}
}

void TurretHead::UpdateIdle(float deltaTime)
{
	if (CheckForTeleport())
	{
		ChangeState(TurretState::Falling);
	}
	else
	{
		if (CheckTargetAcquired())
		{
			ChangeState(TurretState::Priming);
		}
	}
}

void TurretHead::UpdateSearch(float deltaTime)
{
	if (CheckForTeleport())
	{
		ChangeState(TurretState::Falling);
	}
	else
	{

		if (CheckTargetAcquired())
		{
			ChangeState(TurretState::Priming);
		}

		if (mStateTimer > 5.0f)
		{
			ChangeState(TurretState::Idle);
		}

		if (!mStartedSearch)
		{
			mCenterPoint = mPosition + 200.0f * Vector3::UnitX;
			float randAngle = Random::GetFloatRange((-Math::Pi), Math::Pi);
			Vector3 offset =
				Vector3(0.0f, 75.0f * Math::Cos(randAngle), 25.0f * Math::Sin(randAngle));
			mTargetPoint = mCenterPoint + offset;
			mStartedSearch = true;
			Vector3 dir = Vector3::Normalize((mTargetPoint - mPosition));
			float dotProduct = Vector3::Dot(Vector3::UnitX, dir);
			Vector3 normalizedAxis = Vector3::Normalize(Vector3::Cross(Vector3::UnitX, dir));
			float angle = Math::Acos(dotProduct);
			mToRotate = Quaternion(normalizedAxis, angle);
			mBackToCenter = false;
			mToTarget = false;
			mSearchTime = 0.0f;
			mPrevQuat = Quaternion::Identity;
		}
		mSearchTime += deltaTime;
		if (!mToTarget)
		{
			if (mSearchTime < 0.5f)
			{
				SetQuat(Quaternion::Slerp(mPrevQuat, mToRotate, mSearchTime / 0.5f));
			}
			if (mSearchTime >= 0.5f)
			{
				mToTarget = true;
				mSearchTime = 0.0f;
			}
		}
		else if (!mBackToCenter)
		{
			if (mSearchTime < 0.5f)
			{
				SetQuat(Quaternion::Slerp(mToRotate, mPrevQuat, mSearchTime / 0.5f));
			}
			else if (mSearchTime >= 0.5f)
			{
				mSearchTime = 0.0f;
				mStartedSearch = false;
			}
		}
	}
}

void TurretHead::UpdatePriming(float deltaTime)
{
	if (CheckForTeleport())
	{
		ChangeState(TurretState::Falling);
	}
	else
	{
		if (mAcquiredTarget != mLaserComponent->GetLastHitActor())
		{
			ChangeState(TurretState::Search);
		}
		else if (mStateTimer >= 1.5f)
		{
			ChangeState(TurretState::Firing);
		}
	}
}

void TurretHead::UpdateFiring(float deltaTime)
{
	if (CheckForTeleport())
	{
		ChangeState(TurretState::Falling);
	}
	else
	{
		if (mAcquiredTarget != mLaserComponent->GetLastHitActor())
		{
			ChangeState(TurretState::Search);
		}
		if (!mAcquiredTarget->GetComponent<HealthComponent>()->IsDead())
		{
			mFiringCooldown -= deltaTime;
			if (mFiringCooldown <= 0.0f)
			{
				mGame->GetAudio()->PlaySound("Bullet.ogg", false, mAcquiredTarget);
				mAcquiredTarget->GetComponent<HealthComponent>()->TakeDamage(mFiringDamage,
																			 GetWorldPosition());
				mFiringCooldown = 0.05f;
			}
		}
	}
}

void TurretHead::UpdateFalling(float deltaTime)
{
	mParent->SetPosition((mParent->GetPosition() + mFallingVelocity * deltaTime));
	if (!CheckForTeleport())
	{
		mFallingVelocity += GRAVITY * deltaTime;
		CollisionComponent* collComp = mParent->GetComponent<CollisionComponent>();
		for (auto collider : mGame->GetColliders())
		{
			if (collider->GetComponent<CollisionComponent>() != collComp)
			{
				Vector3 offset;
				CollSide collisionSide =
					collComp->GetMinOverlap(collider->GetComponent<CollisionComponent>(), offset);
				if (collisionSide != CollSide::None)
				{
					mParent->SetPosition((mParent->GetPosition() + offset));
					if (collisionSide == CollSide::Top && mFallingVelocity.z < 0.0f)
					{
						Vector3 pos = mParent->GetPosition();
						pos.z -= 15.0f;
						if (dynamic_cast<TurretBase*>(collider))
						{
							dynamic_cast<TurretBase*>(collider)->Die();
							pos.z -=
								collider->GetComponent<CollisionComponent>()->GetHeight() / 2.0f;
						}
						mParent->SetPosition(pos);
						Die();
					}
				}
			}
		}
	}
	if (mFallingVelocity.Length() > 800.0f)
	{
		mFallingVelocity = Vector3::Normalize(mFallingVelocity) * 800.0f;
	}
}

void TurretHead::UpdateDead(float deltaTime)
{
	if (CheckForTeleport())
	{
		ChangeState(TurretState::Falling);
	}
	else
	{
	}
}

void TurretHead::ChangeState(TurretState newState)
{
	if (newState != mTurretState)
	{
		if (mGame->GetAudio()->GetSoundState(mPlayingSound) == SoundState::Playing)
		{
			mGame->GetAudio()->StopSound(mPlayingSound);
		}
		mPlayingSound = mGame->GetAudio()->PlaySound(mStateSounds[newState], false, this);
	}
	mTurretState = newState;
	mStateTimer = 0.0f;
}

bool TurretHead::CheckTargetAcquired()
{
	Actor* lastHit = mLaserComponent->GetLastHitActor();
	if (lastHit)
	{
		if (lastHit->GetComponent<HealthComponent>() &&
			!lastHit->GetComponent<HealthComponent>()->IsDead())
		{
			mAcquiredTarget = lastHit;
			return true;
		}
	}
	return false;
}

bool TurretHead::CheckForTeleport()
{
	Portal* blue = mGame->GetBluePortal();
	Portal* orange = mGame->GetOrangePortal();
	Portal* exitPortal = nullptr;
	CollisionComponent* collComp = GetParent()->GetComponent<CollisionComponent>();
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
		if (intersection && (!mTeleported || mTeleportTimer <= 0.0f))
		{
			GetParent()->SetPosition(exitPortal->GetPosition());
			mFallingVelocity += exitPortal->GetQuatForward() * 250.0f;
			mTeleported = true;
			mTeleportTimer = 0.25f;
			return true;
		}
	}
	return false;
}

void TurretHead::Die()
{
	ChangeState(TurretState::Dead);
	mParent->SetQuat(Quaternion(Vector3::UnitX, Math::PiOver2));
	mLaserComponent->SetDisabled(true);
}

void TurretHead::TakeDamage()
{
	if (!mTakenDamage)
	{
		mTakenDamage = true;
		if (mGame->GetAudio()->GetSoundState(mPlayingSound) == SoundState::Playing)
		{
			mGame->GetAudio()->StopSound(mPlayingSound);
		}
		mPlayingSound = mGame->GetAudio()->PlaySound("TurretFriendlyFire.ogg", false, this);
	}
}