#pragma once
#include "Actor.h"

class Portal : public Actor
{
public:
	Portal(class Game* game, bool orange);
	void SetCollision(Vector3 inSize);
	void OnUpdate(float deltaTime) override;
	float GetOutYaw() const { return mOutYaw; };
	Vector3 CalcOutViewDir(Vector3 inDir, Portal* outPortal);

private:
	Matrix4 CalcViewMatrixBasic(Portal* outPortal);
	Matrix4 CalcViewMatrix(Portal* outPortal);
	Matrix4 OutViewDirection(Portal* outPortal);
	Matrix4 mViewMatrix = Matrix4::CreateScale(0.0f);
	float mOutYaw = 0.0f;
};