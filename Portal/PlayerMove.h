#pragma once
#include "MoveComponent.h"
#include "CollisionComponent.h"
#include "AudioSystem.h"

class PlayerMove : public MoveComponent
{
public:
	PlayerMove(class Actor* owner);
	~PlayerMove();
	void Update(float deltaTime) override;
	void ProcessInput(const Uint8* keyState, Uint32 mouseButtons,
					  const Vector2& relativeMouse) override;
	enum MoveState
	{
		OnGround,
		Jump,
		Falling
	};

private:
	MoveState mCurrentState;
	void ChangeState(MoveState newState) { mCurrentState = newState; }
	void UpdateOnGround(float deltaTime);
	void UpdateOnJump(float deltaTime);
	void UpdateOnFalling(float deltaTime);

	Vector3 mVelocity;
	Vector3 mAcceleration;
	Vector3 mPendingForces;
	float mMass = 1.0f;
	void PhysicsUpdate(float deltaTime);
	void AddForce(const Vector3& force);
	static constexpr float mForceScalar = 700.0f;

	void FixXYVelocity();
	static constexpr float mMaxSpeed = 400.0f;
	static constexpr float mBrakingFactor = 0.9f;
	static constexpr float mNearZero = 0.01f;

	Vector3 mGravity = Vector3(0.0f, 0.0f, -980.0f);

	CollSide FixCollision(CollisionComponent* self, CollisionComponent* collider);

	Vector3 mJumpForce = Vector3(0.0f, 0.0f, 35000.0f);
	bool mSpacePressed = false;

	class Crosshair* mCrosshair;

	bool mLeftClicked = false;
	bool mRightClicked = false;
	void CreatePortal(bool orange);

	const Vector3 NEAR_PLANE_UNPROJECT = Vector3(0.0f, 0.0f, 0.0f);
	const Vector3 FAR_PLANE_UNPROJECT = Vector3(0.0f, 0.0f, 0.9f);
	static constexpr float mLineSegmentMagnitude = 1000.0f;
	static constexpr float mTerminalZVelocity = -1000.0f;
	static constexpr float mReloadThreshold = -750.0f;

	bool mRPressed = false;

	void ResetPortals();

	const Vector3 X_COLLISION_SIZE = Vector3(110.0f, 125.0f, 10.0f);
	const Vector3 Y_COLLISION_SIZE = Vector3(10.0f, 125.0f, 110.0f);
	const Vector3 Z_COLLISION_SIZE = Vector3(110.0f, 10.0f, 125.0f);

	bool UpdatePortalTeleport(float deltaTime);
	float mPortalCooldown = 2.0f;
	bool mTeleported = false;
	static constexpr float mMinExitSpeed = 350.0f;
	bool mFallingFromTeleport = false;

	SoundHandle mWalkingSound;
};