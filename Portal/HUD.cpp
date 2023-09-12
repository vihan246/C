#include "HUD.h"
#include "Actor.h"
#include "Shader.h"
#include "Font.h"
#include "Texture.h"
#include "Game.h"
#include "Renderer.h"
#include "Player.h"
#include "HealthComponent.h"

HUD::HUD(Actor* owner)
: UIComponent(owner)
{
	mFont = new Font();
	mFont->Load("Assets/Inconsolata-Regular.ttf");
	mDamageIndicator =
		GetGame()->GetRenderer()->GetTexture("Assets/Textures/UI/DamageIndicator.png");
}

HUD::~HUD()
{
	mFont->Unload();
	delete mFont;
}

void HUD::Draw(Shader* shader)
{
	if (mSubtitle)
	{
		float height = (float)mSubtitle->GetHeight();
		float yPos = (-325.0f + height / 2.0f);
		Vector2 screenPos = Vector2(0.0f, yPos);
		DrawTexture(shader, mSubShadow, (screenPos + SUB_OFFSET));
		DrawTexture(shader, mSubtitle, screenPos);
	}
	if (mDamageIndicatorTime > 0.0f)
	{
		DrawTexture(shader, mDamageIndicator, Vector2::Zero, 1.0f, mDamageIndicatorAngle);
	}
	if (GetGame()->GetPlayer()->GetComponent<HealthComponent>()->IsDead())
	{
		DrawTexture(shader,
					GetGame()->GetRenderer()->GetTexture("Assets/Textures/UI/DamageOverlay.png"));
	}
}

void HUD::ShowSubtitle(std::string subtitle)
{
	if (mSubtitle)
	{
		mSubtitle->Unload();
		mSubShadow->Unload();
		delete mSubtitle;
		delete mSubShadow;
		mSubtitle = nullptr;
		mSubShadow = nullptr;
	}

	if (!subtitle.empty())
	{
		std::string toShow = "GLaDOS: " + subtitle;
		mSubtitle = mFont->RenderText(toShow, Color::LightGreen);
		mSubShadow = mFont->RenderText(toShow, Color::Black);
	}
}

void HUD::PlayerTakeDamage(float inAngle)
{
	mDamageIndicatorAngle = inAngle;
	mDamageIndicatorTime = mDamageMaxTime;
}

void HUD::Update(float deltaTime)
{
	mDamageIndicatorTime -= deltaTime;
}