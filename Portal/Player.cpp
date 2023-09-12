#include "Player.h"
#include "Game.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "CollisionComponent.h"
#include "PlayerMove.h"
#include "Random.h"
#include "CameraComponent.h"
#include "PlayerMesh.h"
#include "HealthComponent.h"
#include "HUD.h"

Player::Player(Game* game)
: Actor(game)
{
	mHUD = new HUD(this);
	new PlayerMove(this);
	new CameraComponent(this);
	CollisionComponent* collComponent = new CollisionComponent(this);
	collComponent->SetSize(mPlayerWidth, mPlayerHeight, mPlayerDepth);
	HealthComponent* hc = new HealthComponent(this);
	hc->SetOnDeath([this] {
		int toPlay = Random::GetIntRange(0, 3);
		mDeathSound = mGame->GetAudio()->PlaySound(mTauntSounds[toPlay]);
		mHUD->ShowSubtitle(mTauntSubtitles[toPlay]);
	});
	hc->SetOnDamage([this](const Vector3& fromVec) {
		Vector3 playerToDamage = fromVec - GetPosition();
		playerToDamage.z = 0.0f;
		playerToDamage = Vector3::Normalize(playerToDamage);
		Vector3 playerForward = GetForward();
		playerForward.z = 0.0f;
		playerForward = Vector3::Normalize(playerForward);

		float angle = Math::Acos(Vector3::Dot(playerForward, playerToDamage));
		Vector3 cross = Vector3::Cross(playerToDamage, playerForward);

		if (cross.z < 0)
		{
			angle *= -1;
		}
		mHUD->PlayerTakeDamage(angle);
	});
}

void Player::OnUpdate(float deltaTime)
{
}

void Player::GivePlayerGun()
{
	new PlayerMesh(GetGame());
	mPlayerHasGun = true;
}