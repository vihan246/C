#pragma once
#include "UIComponent.h"
#include <string>

class HUD : public UIComponent
{
public:
	HUD(class Actor* owner);
	~HUD();
	void Draw(class Shader* shader) override;
	void ShowSubtitle(std::string subtitle);
	void PlayerTakeDamage(float inAngle);
	void Update(float deltaTime) override;

private:
	class Font* mFont = nullptr;
	class Texture* mSubtitle = nullptr;
	class Texture* mSubShadow = nullptr;
	const Vector2 SUB_OFFSET = Vector2(-2.0f, -2.0f);
	class Texture* mDamageIndicator = nullptr;
	float mDamageIndicatorAngle = 0.0f;
	float mDamageIndicatorTime = 0.0f;
	static float constexpr mDamageMaxTime = 1.5f;
};