#pragma once
#include "Component.h"
#include <functional>

class HealthComponent : public Component
{
public:
	HealthComponent(class Actor* owner, float startingHealth = 100.0f);
	void SetOnDamage(std::function<void(const Vector3&)> inFunction) { mOnDamage = inFunction; }
	void SetOnDeath(std::function<void()> inFunction) { mOnDeath = inFunction; };
	float GetHealth() const { return mHealth; }
	bool IsDead() const { return (mHealth <= 0.0f); }
	void TakeDamage(float damage, const Vector3& dealtFrom);

private:
	float mHealth;
	std::function<void(const Vector3&)> mOnDamage;
	std::function<void()> mOnDeath;
};