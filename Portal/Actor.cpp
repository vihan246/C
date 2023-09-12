#include "Actor.h"
#include "Game.h"
#include "Component.h"
#include <algorithm>

Actor::Actor(Game* game, Actor* parent)
: mGame(game)
, mState(ActorState::Active)
, mPosition(Vector3::Zero)
, mScale(1.0f)
, mRotation(0.0f)
, mRollAngle(0.0f)
{
	mParent = parent;
	if (!mParent)
	{
		mGame->AddActor(this);
	}
	else
	{
		mParent->AddChild(this);
	}
}
void Actor::SetScale(float scale)
{
	mScale.x = scale;
	mScale.y = scale;
	mScale.z = scale;
}

Actor::~Actor()
{
	mGame->GetAudio()->RemoveActor(this);
	// TODO
	if (!mParent)
	{
		mGame->RemoveActor(this);
	}
	else
	{
		mParent->RemoveChild(this);
	}
	for (auto itr : mComponents)
	{
		delete itr;
	}
	while (!mChildren.empty())
	{
		delete mChildren.back();
	}
	mComponents.clear();
}

void Actor::Update(float deltaTime)
{
	// TODO
	CalcWorldTransform();
	if (mState == ActorState::Active)
	{
		for (auto itr : mComponents)
		{
			itr->Update(deltaTime);
		}
		OnUpdate(deltaTime);
	}
	for (auto itr : mChildren)
	{
		itr->Update(deltaTime);
	}
	CalcWorldTransform();
}

void Actor::OnUpdate(float deltaTime)
{
}

void Actor::ProcessInput(const Uint8* keyState, Uint32 mouseButtons, const Vector2& relativeMouse)
{
	// TODO
	if (mState == ActorState::Active)
	{
		for (auto itr : mComponents)
		{
			itr->ProcessInput(keyState, mouseButtons, relativeMouse);
		}
		OnProcessInput(keyState, mouseButtons, relativeMouse);
	}
}

void Actor::OnProcessInput(const Uint8* keyState, Uint32 mouseButtons, const Vector2& relativeMouse)
{
}

void Actor::AddComponent(Component* c)
{
	mComponents.emplace_back(c);
	std::sort(mComponents.begin(), mComponents.end(), [](Component* a, Component* b) {
		return a->GetUpdateOrder() < b->GetUpdateOrder();
	});
}

Vector3 Actor::GetForward() const
{
	float xDirection = cos(mRotation);
	float yDirection = sin(mRotation);
	return Vector3(xDirection, yDirection, 0.0f);
}

Vector3 Actor::GetRight() const
{
	float xDirection = cos(mRotation + Math::PiOver2);
	float yDirection = sin(mRotation + Math::PiOver2);
	return Vector3(xDirection, yDirection, 0.0f);
}

void Actor::CalcWorldTransform()
{
	Matrix4 scale = Matrix4::CreateScale(mScale);
	Matrix4 rotationZ = Matrix4::CreateRotationZ(mRotation);
	Matrix4 rotationX = Matrix4::CreateRotationX(mRollAngle);
	Matrix4 translate = Matrix4::CreateTranslation(mPosition);
	mWorldTransform =
		scale * rotationZ * rotationX * Matrix4::CreateFromQuaternion(mQuat) * translate;
	if (mParent)
	{
		if (mInheritScale)
		{
			mWorldTransform *= mParent->GetWorldTransform();
		}
		else
		{
			mWorldTransform *= mParent->GetWorldRotTrans();
		}
	}
}
Vector3 Actor::GetQuatForward() const
{
	Vector3 fwd = Vector3::Transform(Vector3::UnitX, mQuat);
	fwd.Normalize();
	return fwd;
}

void Actor::AddChild(Actor* actor)
{
	mChildren.push_back(actor);
}

void Actor::RemoveChild(Actor* actor)
{
	auto iter = std::find(mChildren.begin(), mChildren.end(), actor);
	if (iter != mChildren.end())
	{
		mChildren.erase(iter);
	}
}

Matrix4 Actor::GetWorldRotTrans() const
{
	Matrix4 rotationZ = Matrix4::CreateRotationZ(mRotation);
	Matrix4 rotationX = Matrix4::CreateRotationX(mRollAngle);
	Matrix4 translate = Matrix4::CreateTranslation(mPosition);
	Matrix4 output = rotationZ * rotationX * Matrix4::CreateFromQuaternion(mQuat) * translate;
	if (mParent)
	{
		output *= mParent->GetWorldRotTrans();
	}
	return output;
}