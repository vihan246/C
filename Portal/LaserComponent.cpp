#include "LaserComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "SegmentCast.h"
#include "Portal.h"

LaserComponent::LaserComponent(Actor* owner)
: MeshComponent(owner, true)
{
	SetMesh(owner->GetGame()->GetRenderer()->GetMesh("Assets/Meshes/Laser.gpmesh"));
}

void LaserComponent::Update(float deltaTime)
{
	mLineSegments.clear();
	if (!mDisabled)
	{
		LineSegment ls =
			LineSegment(GetOwner()->GetWorldPosition(),
						(GetOwner()->GetWorldPosition() + GetOwner()->GetWorldForward() * 350.0f));
		CastInfo cs;
		if (SegmentCast(mOwner->GetGame()->GetActors(), ls, cs, mIgnoreActor))
		{
			ls.mEnd = cs.mPoint;
			mLastHitActor = cs.mActor;
			if (dynamic_cast<Portal*>(cs.mActor))
			{
				Portal* inPortal = dynamic_cast<Portal*>(cs.mActor);
				Portal* outPortal = nullptr;
				if (inPortal == GetGame()->GetBluePortal())
				{
					outPortal = GetGame()->GetOrangePortal();
				}
				else if (inPortal == GetGame()->GetOrangePortal())
				{
					outPortal = GetGame()->GetBluePortal();
				}

				if (outPortal)
				{
					Vector3 outPortalDir =
						inPortal->CalcOutViewDir(GetOwner()->GetWorldForward(), outPortal);
					Vector3 start = (outPortal->GetPosition() + outPortalDir * 0.5f);
					Vector3 end = start + outPortalDir * 350.0f;
					LineSegment second = LineSegment(start, end);
					CastInfo csSecond;
					if (SegmentCast(mOwner->GetGame()->GetActors(), second, csSecond, outPortal))
					{
						second.mEnd = csSecond.mPoint;
						mLastHitActor = csSecond.mActor;
					}
					else
					{
						mLastHitActor = nullptr;
					}
					mLineSegments.push_back(second);
				}
			}
		}
		else
		{
			mLastHitActor = nullptr;
		}
		mLineSegments.push_back(ls);
	}
}

void LaserComponent::Draw(Shader* shader)
{
	for (size_t i = 0; i < mLineSegments.size(); i++)
	{
		if (mMesh)
		{
			// Set the world transform
			shader->SetMatrixUniform("uWorldTransform", LaserTransformMatrix(mLineSegments[i]));
			// Set the active texture
			Texture* t = mMesh->GetTexture(mTextureIndex);
			if (t)
			{
				t->SetActive();
			}
			// Set the mesh's vertex array as active
			VertexArray* va = mMesh->GetVertexArray();
			va->SetActive();
			// Draw
			glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
		}
	}
}

Matrix4 LaserComponent::LaserTransformMatrix(LineSegment ls)
{
	Matrix4 scale = Matrix4::CreateScale(ls.Length(), 1.0f, 1.0f);
	float dotProduct = Vector3::Dot(Vector3::UnitX, Vector3::Normalize((ls.mEnd - ls.mStart)));
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
		Vector3 normalizedAxis = Vector3::Normalize(
			Vector3::Cross(Vector3::UnitX, Vector3::Normalize((ls.mEnd - ls.mStart))));
		float angle = Math::Acos(dotProduct);
		rotationQuat = Quaternion(normalizedAxis, angle);
	}
	Matrix4 rotation = Matrix4::CreateFromQuaternion(rotationQuat);
	Matrix4 translation = Matrix4::CreateTranslation(ls.PointOnSegment(0.5f));

	return (scale * rotation * translation);
}