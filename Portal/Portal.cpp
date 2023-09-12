#include "Portal.h"
#include "Game.h"
#include "PortalMeshComponent.h"
#include "CollisionComponent.h"
#include "Renderer.h"
#include "Player.h"

Portal::Portal(Game* game, bool orange)
: Actor(game)
{
	PortalMeshComponent* pmc = new PortalMeshComponent(this);
	pmc->SetTextureIndex((int)orange);

	new CollisionComponent(this);
}

void Portal::SetCollision(Vector3 inSize)
{
	GetComponent<CollisionComponent>()->SetSize(inSize.x, inSize.y, inSize.z);
}

void Portal::OnUpdate(float deltaTime)
{
	if (this == mGame->GetBluePortal())
	{
		mGame->GetRenderer()->GetBluePortal().mView = CalcViewMatrix(mGame->GetOrangePortal());
	}
	else
	{
		mGame->GetRenderer()->GetOrangePortal().mView = CalcViewMatrix(mGame->GetBluePortal());
	}
}

Matrix4 Portal::CalcViewMatrixBasic(Portal* outPortal)
{
	if (outPortal)
	{
		Vector3 eye = outPortal->GetPosition() - outPortal->GetQuatForward() * 5.0f;
		Vector3 target = outPortal->GetPosition() + outPortal->GetQuatForward() * 5.0f;
		Vector3 up = outPortal->GetWorldTransform().GetZAxis();
		return Matrix4::CreateLookAt(eye, target, up);
	}
	return Matrix4::CreateScale(0.0f);
}

Matrix4 Portal::CalcViewMatrix(Portal* outPortal)
{
	if (outPortal)
	{
		if (Math::Abs(GetQuatForward().z) > 0.1f)
		{
			return CalcViewMatrixBasic(outPortal);
		}
		return OutViewDirection(outPortal);
	}
	return Matrix4::CreateScale(0.0f);
}

Matrix4 Portal::OutViewDirection(Portal* outPortal)
{
	Vector3 playerToIn = mPosition - mGame->GetPlayer()->GetPosition();
	float inDist = playerToIn.Length();
	if (inDist == 0.0f)
	{
		return mViewMatrix;
	}
	else
	{
		playerToIn = Vector3::Normalize(playerToIn);
		Matrix4 inverseWT = GetWorldTransform();
		inverseWT.Invert();
		playerToIn = Vector3::Transform(playerToIn, inverseWT, 0.0f);
		Matrix4 zRot = Matrix4::CreateRotationZ(Math::Pi);
		playerToIn = Vector3::Transform(playerToIn, zRot, 0.0f);
		playerToIn = Vector3::Transform(playerToIn, outPortal->GetWorldTransform(), 0.0f);
		Vector3 outDir = Vector3::Normalize(playerToIn);

		Vector3 eye = outPortal->GetPosition() - (outDir * 5.0f);
		Vector3 target = outPortal->GetPosition() + (outDir * 5.0f);
		Vector3 up = outPortal->GetWorldTransform().GetZAxis();
		mViewMatrix = Matrix4::CreateLookAt(eye, target, up);

		outDir.z = 0.0f;
		outDir = Vector3::Normalize(outDir);

		float dotProduct = Vector3::Dot(Vector3::UnitX, outDir);
		Vector3 crossProduct = Vector3::Cross(Vector3::UnitX, outDir);
		float rotAngle = Math::Acos(dotProduct);

		if (crossProduct.z < 0)
		{
			rotAngle *= -1;
		}
		mOutYaw = rotAngle;

		return mViewMatrix;
	}
}

Vector3 Portal::CalcOutViewDir(Vector3 inDir, Portal* outPortal)
{
	Vector3 toCalc = inDir;
	toCalc = Vector3::Normalize(toCalc);
	Matrix4 inverseWT = GetWorldTransform();
	inverseWT.Invert();
	Vector3 playerToInObj = Vector3::Transform(toCalc, inverseWT, 0.0f);
	Matrix4 zRot = Matrix4::CreateRotationZ(Math::Pi);
	Vector3 outObj = Vector3::Transform(playerToInObj, zRot, 0.0f);
	Vector3 outWorld = Vector3::Transform(outObj, outPortal->GetWorldTransform(), 0.0f);
	outWorld = Vector3::Normalize(outWorld);
	return outWorld;
}