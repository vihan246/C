#include "CameraComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"

CameraComponent::CameraComponent(Actor* owner)
: Component(owner)
{
	mCameraVelocity = Vector3(0, 0, 0);
	mCameraPosition = owner->GetPosition();
}

void CameraComponent::Update(float deltaTime)
{
	mPitchAngle += mPitchSpeed * deltaTime;
	mPitchAngle = Math::Clamp(mPitchAngle, -Math::Pi / 2.1f, Math::Pi / 2.1f);
	Matrix4 pitchRotation = Matrix4::CreateRotationY(mPitchAngle);
	Matrix4 yawRotation = Matrix4::CreateRotationZ(mOwner->GetRotation());
	Matrix4 combinedRotMatrix = pitchRotation * yawRotation;
	Vector3 cameraForward = Vector3::Transform(Vector3::UnitX, combinedRotMatrix);

	Vector3 targetPos = mOwner->GetPosition();
	targetPos += cameraForward * mTargetOffset;
	Matrix4 newView = Matrix4::CreateLookAt(mOwner->GetPosition(), targetPos, Vector3::UnitZ);
	mOwner->GetGame()->GetRenderer()->SetViewMatrix(newView);
}

Vector3 CameraComponent::CalculateIdealPos()
{
	Vector3 newPos = mOwner->GetPosition();
	Vector3 cameraPos = newPos;
	cameraPos -= (mOwner->GetForward() * mHorizontalDistance);
	cameraPos.z = idealZ;
	return cameraPos;
}
