#include "PlayerMove.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include "Random.h"
#include "CollisionComponent.h"
#include "Player.h"
#include "AudioSystem.h"
#include "CameraComponent.h"
#include "Crosshair.h"
#include "SegmentCast.h"
#include "Block.h"
#include "Portal.h"
#include "HealthComponent.h"

PlayerMove::PlayerMove(Actor* owner)
: MoveComponent(owner)
{
	ChangeState(MoveState::Falling);
	mCrosshair = new Crosshair(owner);
	mCurrentState = MoveState::OnGround;
	mWalkingSound = GetGame()->GetAudio()->PlaySound("FootstepLoop.ogg", true);
	GetGame()->GetAudio()->PauseSound(mWalkingSound);
}

PlayerMove::~PlayerMove()
{
	GetGame()->GetAudio()->StopSound(mWalkingSound);
}

void PlayerMove::Update(float deltaTime)
{
	if (mOwner->GetComponent<HealthComponent>()->IsDead())
	{
		if (GetGame()->GetAudio()->GetSoundState(GetGame()->GetPlayer()->GetDeathSound()) ==
			SoundState::Stopped)
		{
			GetGame()->Reload();
		}
		else
		{
			return;
		}
	}
	switch (mCurrentState)
	{
	case MoveState::OnGround:
		UpdateOnGround(deltaTime);
		break;
	case MoveState::Jump:
		UpdateOnJump(deltaTime);
		break;
	case MoveState::Falling:
		UpdateOnFalling(deltaTime);
		break;
	}
	if (GetOwner()->GetPosition().z <= mReloadThreshold)
	{
		mOwner->GetComponent<HealthComponent>()->TakeDamage(Math::Infinity, mOwner->GetPosition());
	}

	if (mCurrentState == MoveState::OnGround && mVelocity.Length() >= 50.0f)
	{
		GetGame()->GetAudio()->ResumeSound(mWalkingSound);
	}
	else
	{
		GetGame()->GetAudio()->PauseSound(mWalkingSound);
	}
}

void PlayerMove::ProcessInput(const Uint8* keyState, Uint32 mouseButtons,
							  const Vector2& relativeMouse)
{
	float forwardForceScalar = 0.0f;
	if (keyState[SDL_SCANCODE_W])
	{
		forwardForceScalar += mForceScalar;
	}
	if (keyState[SDL_SCANCODE_S])
	{
		forwardForceScalar -= mForceScalar;
	}
	AddForce((mOwner->GetForward() * forwardForceScalar));
	float strafeForceScalar = 0.0f;
	if (keyState[SDL_SCANCODE_A])
	{
		strafeForceScalar -= mForceScalar;
	}
	if (keyState[SDL_SCANCODE_D])
	{
		strafeForceScalar += mForceScalar;
	}
	AddForce((mOwner->GetRight() * strafeForceScalar));
	float angularSpeed = relativeMouse.x / 500.0f * Math::Pi * 10.0f;
	SetAngularSpeed(angularSpeed);

	float pitchSpeed = relativeMouse.y / 500.0f * Math::Pi * 10.0f;
	mOwner->GetComponent<CameraComponent>()->SetPitchSpeed(pitchSpeed);

	if (keyState[SDL_SCANCODE_SPACE])
	{
		if (!mSpacePressed && mCurrentState == MoveState::OnGround)
		{
			AddForce(mJumpForce);
			GetGame()->GetAudio()->PlaySound("Jump.ogg");
			ChangeState(MoveState::Jump);
		}
		mSpacePressed = true;
	}
	else
	{
		mSpacePressed = false;
	}

	bool leftPressed = mouseButtons & SDL_BUTTON(SDL_BUTTON_LEFT);
	bool rightPressed = mouseButtons & SDL_BUTTON(SDL_BUTTON_RIGHT);

	if (leftPressed)
	{
		if (!mLeftClicked && GetGame()->GetPlayer()->HasGun())
		{
			CreatePortal(false);
		}
		mLeftClicked = true;
	}
	else
	{
		mLeftClicked = false;
	}

	if (rightPressed)
	{
		if (!mRightClicked && GetGame()->GetPlayer()->HasGun())
		{
			CreatePortal(true);
		}
		mRightClicked = true;
	}
	else
	{
		mRightClicked = false;
	}

	if (keyState[SDL_SCANCODE_R])
	{
		if (!mRPressed)
		{
			ResetPortals();
		}
		mRPressed = true;
	}
	else
	{
		mRPressed = false;
	}
}

void PlayerMove::UpdateOnGround(float deltaTime)
{
	PhysicsUpdate(deltaTime);
	if (UpdatePortalTeleport(deltaTime))
	{
		return;
	}
	bool noneTop = true;
	for (auto collider : GetGame()->GetColliders())
	{
		CollSide minSide = FixCollision(mOwner->GetComponent<CollisionComponent>(),
										collider->GetComponent<CollisionComponent>());
		if (minSide == CollSide::Top)
		{
			noneTop = false;
		}
	}
	if (noneTop)
	{
		ChangeState(MoveState::Falling);
	}
}

void PlayerMove::UpdateOnJump(float deltaTime)
{
	AddForce(mGravity);
	PhysicsUpdate(deltaTime);
	if (UpdatePortalTeleport(deltaTime))
	{
		return;
	}
	for (auto collider : GetGame()->GetColliders())
	{
		CollSide minSide = FixCollision(mOwner->GetComponent<CollisionComponent>(),
										collider->GetComponent<CollisionComponent>());
		if (minSide == CollSide::Bottom)
		{
			mVelocity.z = 0.0f;
		}
	}
	if (mVelocity.z <= 0.0f)
	{
		ChangeState(MoveState::Falling);
	}
}

void PlayerMove::UpdateOnFalling(float deltaTime)
{
	AddForce(mGravity);
	PhysicsUpdate(deltaTime);
	if (UpdatePortalTeleport(deltaTime))
	{
		return;
	}
	bool landed = false;
	for (auto collider : GetGame()->GetColliders())
	{
		CollSide minSide = FixCollision(mOwner->GetComponent<CollisionComponent>(),
										collider->GetComponent<CollisionComponent>());
		if (minSide == CollSide::Top && mVelocity.z <= 0.0f)
		{
			landed = true;
		}
	}
	if (landed)
	{
		mVelocity.z = 0.0f;
		GetGame()->GetAudio()->PlaySound("Land.ogg");
		ChangeState(OnGround);
		mFallingFromTeleport = false;
	}
}

void PlayerMove::AddForce(const Vector3& force)
{
	mPendingForces += force;
}

void PlayerMove::PhysicsUpdate(float deltaTime)
{
	mAcceleration = mPendingForces * (1.0f / mMass);
	mVelocity += mAcceleration * deltaTime;
	if (!mFallingFromTeleport)
	{
		FixXYVelocity();
	}
	if (mVelocity.z < mTerminalZVelocity)
	{
		mVelocity.z = mTerminalZVelocity;
	}
	Vector3 position = mOwner->GetPosition();
	position += mVelocity * deltaTime;
	mOwner->SetPosition(position);

	float rotation = mOwner->GetRotation();
	rotation += mAngularSpeed * deltaTime;
	mOwner->SetRotation(rotation);

	mPendingForces = Vector3::Zero;
}

void PlayerMove::FixXYVelocity()
{
	Vector2 xyVelocity = Vector2(mVelocity.x, mVelocity.y);
	if (xyVelocity.Length() > mMaxSpeed)
	{
		xyVelocity = Vector2::Normalize(xyVelocity) * mMaxSpeed;
	}
	if (mCurrentState == MoveState::OnGround)
	{
		if ((mAcceleration.x >= -mNearZero && mAcceleration.x <= mNearZero) ||
			(mAcceleration.x * mVelocity.x < 0.0f))
		{
			xyVelocity.x *= mBrakingFactor;
		}
		if ((mAcceleration.y >= -mNearZero && mAcceleration.y <= mNearZero) ||
			(mAcceleration.y * mVelocity.y < 0.0f))
		{
			xyVelocity.y *= mBrakingFactor;
		}
	}

	mVelocity.x = xyVelocity.x;
	mVelocity.y = xyVelocity.y;
}

CollSide PlayerMove::FixCollision(CollisionComponent* self, CollisionComponent* collider)
{
	Vector3 offset;
	CollSide minSide = self->GetMinOverlap(collider, offset);

	if (minSide != CollSide::None)
	{
		Vector3 position = mOwner->GetPosition();
		position += offset;
		mOwner->SetPosition(position);
		if (minSide == CollSide::Front)
		{
			AddForce((Vector3::UnitX * mForceScalar));
		}
		else if (minSide == CollSide::Back)
		{
			AddForce((Vector3::UnitX * -mForceScalar));
		}
		else if (minSide == CollSide::Left)
		{
			AddForce((Vector3::UnitY * -mForceScalar));
		}
		else if (minSide == CollSide::Right)
		{
			AddForce((Vector3::UnitY * mForceScalar));
		}
	}
	return minSide;
}

void PlayerMove::CreatePortal(bool orange)
{
	Vector3 nearPoint = GetGame()->GetRenderer()->Unproject(NEAR_PLANE_UNPROJECT);
	Vector3 farPoint = GetGame()->GetRenderer()->Unproject(FAR_PLANE_UNPROJECT);
	Vector3 direction = farPoint - nearPoint;
	Vector3 normalizedDirection = Vector3::Normalize(direction);
	Vector3 endLinePoint = nearPoint + normalizedDirection * mLineSegmentMagnitude;
	LineSegment lineSegment = LineSegment(nearPoint, endLinePoint);
	CastInfo castInfo;

	if (SegmentCast(GetGame()->GetColliders(), lineSegment, castInfo))
	{
		if (dynamic_cast<Block*>(castInfo.mActor))
		{
			float dotProduct = Vector3::Dot(Vector3::UnitX, castInfo.mNormal);
			Quaternion rotationQuat;
			if (dotProduct == 1.0f)
			{
				rotationQuat = Quaternion::Identity;
			}
			else if (dotProduct == -1.0f)
			{
				rotationQuat = Quaternion(Vector3::UnitZ, Math::Pi);
			}
			else
			{
				Vector3 normalizedAxis =
					Vector3::Normalize(Vector3::Cross(Vector3::UnitX, castInfo.mNormal));
				float angle = Math::Acos(dotProduct);
				rotationQuat = Quaternion(normalizedAxis, angle);
			}

			Portal* newPortal = new Portal(GetGame(), orange);
			newPortal->SetPosition(castInfo.mPoint);
			newPortal->SetQuat(rotationQuat);
			if (Math::Abs(castInfo.mNormal.x) > 0)
			{
				newPortal->SetCollision(X_COLLISION_SIZE);
			}
			else if (Math::Abs(castInfo.mNormal.y) > 0)
			{
				newPortal->SetCollision(Y_COLLISION_SIZE);
			}
			else
			{
				newPortal->SetCollision(Z_COLLISION_SIZE);
			}
			Portal* currOrangePortal = GetGame()->GetOrangePortal();
			Portal* currBluePortal = GetGame()->GetBluePortal();
			if (orange)
			{
				GetGame()->GetAudio()->PlaySound("PortalShootOrange.ogg");
				if (currOrangePortal)
				{
					currOrangePortal->SetState(ActorState::Destroy);
				}
				GetGame()->SetOrangePortal(newPortal);
				if (currBluePortal)
				{
					mCrosshair->SetState(CrosshairState::BothFill);
				}
				else
				{
					mCrosshair->SetState(CrosshairState::OrangeFill);
				}
			}
			else
			{
				GetGame()->GetAudio()->PlaySound("PortalShootBlue.ogg");
				if (currBluePortal)
				{
					currBluePortal->SetState(ActorState::Destroy);
				}
				GetGame()->SetBluePortal(newPortal);
				if (currOrangePortal)
				{
					mCrosshair->SetState(CrosshairState::BothFill);
				}
				else
				{
					mCrosshair->SetState(CrosshairState::BlueFill);
				}
			}
		}
		else
		{
			GetGame()->GetAudio()->PlaySound("PortalFail.ogg");
		}
	}
	else
	{
		GetGame()->GetAudio()->PlaySound("PortalFail.ogg");
	}
}

void PlayerMove::ResetPortals()
{
	Portal* orangePortal = GetGame()->GetOrangePortal();
	Portal* bluePortal = GetGame()->GetBluePortal();
	if (orangePortal || bluePortal)
	{
		GetGame()->GetAudio()->PlaySound("PortalClose.ogg");
	}
	if (orangePortal)
		orangePortal->SetState(ActorState::Destroy);
	if (bluePortal)
		bluePortal->SetState(ActorState::Destroy);

	GetGame()->SetOrangePortal(nullptr);
	GetGame()->SetBluePortal(nullptr);
	mCrosshair->SetState(CrosshairState::Default);
}

bool PlayerMove::UpdatePortalTeleport(float deltaTime)
{
	if (!mTeleported)
	{
		Portal* blue = GetGame()->GetBluePortal();
		Portal* orange = GetGame()->GetOrangePortal();
		Portal* entryPortal = nullptr;
		Portal* exitPortal = nullptr;
		if ((blue != nullptr) && (orange != nullptr))
		{
			bool intersection = false;
			if (GetOwner()->GetComponent<CollisionComponent>()->Intersect(
					blue->GetComponent<CollisionComponent>()))
			{
				entryPortal = blue;
				exitPortal = orange;
				intersection = true;
			}
			else if (GetOwner()->GetComponent<CollisionComponent>()->Intersect(
						 orange->GetComponent<CollisionComponent>()))
			{
				entryPortal = orange;
				exitPortal = blue;
				intersection = true;
			}
			if (intersection)
			{
				GetGame()->GetAudio()->PlaySound("PortalTeleport.ogg");
				GetOwner()->SetPosition(exitPortal->GetPosition());
				ChangeState(MoveState::Falling);
				float entryVelocityMag =
					Vector3::Dot(mVelocity, entryPortal->GetQuatForward()) * -1;
				float exitVelocityMag = Math::Max(mMinExitSpeed, 1.5f * entryVelocityMag);
				Vector3 exitVelocity = exitVelocityMag * exitPortal->GetQuatForward();
				mVelocity = exitVelocity;
				mFallingFromTeleport = true;
				mTeleported = true;
				if (Math::NearZero(Math::Abs(exitPortal->GetQuatForward().z)))
				{
					float rotAngle = entryPortal->GetOutYaw();
					GetOwner()->SetRotation(rotAngle);
				}
				return true;
			}
		}
	}
	else
	{
		mPortalCooldown -= deltaTime;
		if (mPortalCooldown <= 0)
		{
			mTeleported = false;
			mPortalCooldown = 2.0f;
		}
	}
	return false;
}