#include "MoveComponent.h"
#include "Actor.h"

MoveComponent::MoveComponent(class Actor* owner)
: Component(owner, 50)
, mAngularSpeed(0.0f)
, mForwardSpeed(0.0f)
, mStrafeSpeed(0.0f)
{
}

void MoveComponent::Update(float deltaTime)
{
	mOwner->SetRotation(mOwner->GetRotation() + (deltaTime * mAngularSpeed));
	Vector3 velocity = mOwner->GetForward();
	velocity = velocity * mForwardSpeed;
	velocity += mOwner->GetRight() * mStrafeSpeed;

	Vector3 currentPosition = mOwner->GetPosition();
	Vector3 newPosition = currentPosition + (velocity * deltaTime);
	mOwner->SetPosition(newPosition);
}
