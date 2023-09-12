#pragma once
#include "Actor.h"
#include <vector>
#include <string>
#include "AudioSystem.h"

class Player : public Actor
{
public:
	Player(class Game* game);
	void OnUpdate(float deltaTime) override;
	bool HasGun() const { return mPlayerHasGun; }
	void GivePlayerGun();
	void SetInitialPosition(Vector3 newPos) { mInitalPosition = newPos; }
	Vector3& GetInitialPosition() { return mInitalPosition; }
	class HUD* GetHUD() { return mHUD; }
	SoundHandle GetDeathSound() const { return mDeathSound; }

private:
	static constexpr float mPlayerWidth = 50.0f;
	static constexpr float mPlayerHeight = 100.0f;
	static constexpr float mPlayerDepth = 50.0f;
	bool mPlayerHasGun = false;
	Vector3 mInitalPosition = Vector3(0, 0, 0);
	class HUD* mHUD = nullptr;
	SoundHandle mDeathSound = 0;
	std::vector<std::string> mTauntSounds = {"Glados-PlayerDead1.ogg", "Glados-PlayerDead2.ogg",
											 "Glados-PlayerDead3.ogg", "Glados-PlayerDead4.ogg"};
	std::vector<std::string> mTauntSubtitles = {
		"Congratulations! The test is now over.",
		"Thank you for participating in this Aperture Science computer-aided enrichment activity.",
		"Goodbye.", "You're not a good person. You know that, right?"};
};