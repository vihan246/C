#pragma once
#include "Component.h"
#include "Math.h"

class CameraComponent : public Component
{
public:
	CameraComponent(class Actor* owner);
	void Update(float deltaTime) override;
	void SnapToIdeal();
	float GetPitchSpeed() const { return mPitchSpeed; }
	void SetPitchSpeed(float speed) { mPitchSpeed = speed; }
	float GetPitchAngle() const { return mPitchAngle; }
	void ResetPitchAngle() { mPitchAngle = 0.0f; }

private:
	float mHorizontalDistance = 60.0f;
	float mTargetOffset = 50.0f;
	Vector3 mCameraPosition;
	Vector3 mCameraVelocity;

	Vector3 CalculateIdealPos();

	static float constexpr idealZ = 70.0f;

	float mPitchAngle = 0.0f;
	float mPitchSpeed = 0.0f;
};