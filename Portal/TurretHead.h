#pragma once
#include <unordered_map>
#include <string>
#include "Actor.h"
#include "AudioSystem.h"

enum class TurretState
{
	Idle,
	Search,
	Priming,
	Firing,
	Falling,
	Dead
};

class TurretHead : public Actor
{
public:
	TurretHead(class Game*, Actor* parent);
	void OnUpdate(float deltaTime) override;
	void UpdateIdle(float deltaTime);
	void UpdateSearch(float deltaTime);
	void UpdatePriming(float deltaTime);
	void UpdateFiring(float deltaTime);
	void UpdateFalling(float deltaTime);
	void UpdateDead(float deltaTime);
	void ChangeState(TurretState newState);
	void Die();
	void TakeDamage();

private:
	Actor* mLaser;
	class LaserComponent* mLaserComponent;
	TurretState mTurretState = TurretState::Idle;
	float mStateTimer = 0.0f;
	bool CheckTargetAcquired();
	Actor* mAcquiredTarget = nullptr;
	bool mStartedSearch = false;
	bool mToTarget = false;
	bool mBackToCenter = false;
	Vector3 mCenterPoint = Vector3(0, 0, 0);
	Vector3 mTargetPoint = Vector3(0, 0, 0);
	Quaternion mToRotate = Quaternion::Identity;
	Quaternion mPrevQuat;
	float mSearchTime = 0.0f;
	Vector3 mFallingVelocity = Vector3::Zero;
	bool CheckForTeleport();
	float mTeleportTimer = 0.25f;
	bool mTeleported = false;
	const Vector3 GRAVITY = Vector3(0.0f, 0.0f, -980.0f);
	float mFiringCooldown = 0.05f;
	static float constexpr mFiringDamage = 2.5f;
	std::unordered_map<TurretState, std::string> mStateSounds = {
		{TurretState::Idle, "TurretIdle.ogg"},		 {TurretState::Search, "TurretSearch.ogg"},
		{TurretState::Priming, "TurretPriming.ogg"}, {TurretState::Firing, "TurretFiring.ogg"},
		{TurretState::Falling, "TurretFalling.ogg"}, {TurretState::Dead, "TurretDead.ogg"}};

	SoundHandle mPlayingSound;
	bool mTakenDamage = false;
};