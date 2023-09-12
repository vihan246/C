#pragma once
#include "Actor.h"
#include <string>
#include <vector>
#include "AudioSystem.h"
class VOTrigger : public Actor
{
public:
	VOTrigger(class Game* game, std::string doorName, std::string nextLevel,
			  std::vector<std::string> sounds, std::vector<std::string> subtitles);
	void OnUpdate(float deltaTime) override;
	void OnProcessInput(const Uint8* keyState, Uint32 mouseButtons,
						const Vector2& relativeMouse) override;

private:
	std::string mDoor;
	std::string mNextLevel;
	std::vector<std::string> mSounds;
	std::vector<std::string> mSubtitles;
	size_t mNextSound = 0;
	SoundHandle mPlayingSound = 0;
	bool mInteracted = false;
	bool mFPressed = false;
	void PlaySoundSubtitle(AudioSystem* audio);
};