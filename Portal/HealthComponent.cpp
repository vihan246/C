#include "HealthComponent.h"
#include "Actor.h"

HealthComponent::HealthComponent(Actor* owner, float startingHealth)
: Component(owner)
{
	mHealth = startingHealth;
}

void HealthComponent::TakeDamage(float damage, const Vector3& dealtFrom)
{
	if (!IsDead())
	{
		mHealth -= damage;
		if (mOnDamage)
		{
			mOnDamage(dealtFrom);
		}
		if (IsDead() && mOnDeath)
		{
			mOnDeath();
		}
	}
}