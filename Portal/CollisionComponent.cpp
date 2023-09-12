#include "CollisionComponent.h"
#include "Actor.h"
#include <algorithm>

CollisionComponent::CollisionComponent(class Actor* owner)
: Component(owner)
, mWidth(0.0f)
, mHeight(0.0f)
, mDepth(0.0f)
{
}

CollisionComponent::~CollisionComponent()
{
}

bool CollisionComponent::Intersect(const CollisionComponent* other) const
{
	Vector3 thisMin = GetMin();
	Vector3 thisMax = GetMax();
	Vector3 otherMin = other->GetMin();
	Vector3 otherMax = other->GetMax();

	bool noIntersection = thisMax.x < otherMin.x || thisMax.y < otherMin.y ||
						  thisMax.z < otherMin.z || otherMax.x < thisMin.x ||
						  otherMax.y < thisMin.y || otherMax.z < thisMin.z;

	return !noIntersection;
}

Vector3 CollisionComponent::GetMin() const
{
	Vector3 v = mOwner->GetPosition();
	v.x -= mDepth * mOwner->GetScale().x / 2.0f;
	v.y -= mWidth * mOwner->GetScale().y / 2.0f;
	v.z -= mHeight * mOwner->GetScale().z / 2.0f;
	return v;
}

Vector3 CollisionComponent::GetMax() const
{
	Vector3 v = mOwner->GetPosition();
	v.x += mDepth * mOwner->GetScale().x / 2.0f;
	v.y += mWidth * mOwner->GetScale().y / 2.0f;
	v.z += mHeight * mOwner->GetScale().z / 2.0f;
	return v;
}

Vector3 CollisionComponent::GetCenter() const
{
	return mOwner->GetPosition();
}

CollSide CollisionComponent::GetMinOverlap(const CollisionComponent* other, Vector3& offset) const
{
	offset = Vector3::Zero;

	if (!Intersect(other))
	{
		return CollSide::None;
	}

	Vector3 otherMax = other->GetMax();
	Vector3 otherMin = other->GetMin();

	float topDist = otherMax.z - GetMin().z;
	float bottomDist = otherMin.z - GetMax().z;
	float leftDist = otherMin.y - GetMax().y;
	float rightDist = otherMax.y - GetMin().y;
	float frontDist = otherMax.x - GetMin().x;
	float backDist = otherMin.x - GetMax().x;

	float xOffset = 0.0f;
	float yOffset = 0.0f;
	float zOffset = 0.0f;
	bool top = true;
	bool front = true;
	bool right = true;

	if (Math::Abs(topDist) < Math::Abs(bottomDist))
	{
		zOffset = topDist;
	}
	else
	{
		zOffset = bottomDist;
		top = false;
	}

	if (Math::Abs(frontDist) < Math::Abs(backDist))
	{
		xOffset = frontDist;
	}
	else
	{
		xOffset = backDist;
		front = false;
	}

	if (Math::Abs(rightDist) < Math::Abs(leftDist))
	{
		yOffset = rightDist;
	}
	else
	{
		yOffset = leftDist;
		right = false;
	}

	float minSide =
		Math::Min(Math::Abs(zOffset), Math::Min(Math::Abs(xOffset), Math::Abs(yOffset)));

	if (minSide == Math::Abs(yOffset))
	{
		xOffset = 0;
		zOffset = 0;
		offset = Vector3(xOffset, yOffset, zOffset);
		return right ? CollSide::Right : CollSide::Left;
	}
	else if (minSide == Math::Abs(xOffset))
	{
		zOffset = 0;
		yOffset = 0;
		offset = Vector3(xOffset, yOffset, zOffset);
		return front ? CollSide::Front : CollSide::Back;
	}
	else if (minSide == Math::Abs(zOffset))
	{
		xOffset = 0;
		yOffset = 0;
		offset = Vector3(xOffset, yOffset, zOffset);
		return top ? CollSide::Top : CollSide::Bottom;
	}

	return CollSide::None;
}
