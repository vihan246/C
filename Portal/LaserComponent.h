#pragma once
#include "MeshComponent.h"
#include "SegmentCast.h"
#include <vector>

class LaserComponent : public MeshComponent
{
public:
	LaserComponent(class Actor* owner);
	void Update(float deltaTime) override;
	void Draw(class Shader* shader) override;
	void SetIgnoreActor(class Actor* actor) { mIgnoreActor = actor; }
	class Actor* GetLastHitActor() { return mLastHitActor; }
	void SetDisabled(bool inBool) { mDisabled = inBool; }

private:
	std::vector<LineSegment> mLineSegments;
	Matrix4 LaserTransformMatrix(LineSegment ls);
	class Actor* mIgnoreActor = nullptr;
	class Actor* mLastHitActor = nullptr;
	bool mDisabled = false;
};