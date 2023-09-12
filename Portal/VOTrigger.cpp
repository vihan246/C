#include "VOTrigger.h"
#include "Game.h"
#include "CollisionComponent.h"
#include "Player.h"
#include "Door.h"
#include "HUD.h"
#include "HealthComponent.h"

VOTrigger::VOTrigger(Game* game, std::string doorName, std::string nextLevel,
					 std::vector<std::string> sounds, std::vector<std::string> subtitles)
: Actor(game)
{
	mDoor = doorName;
	mNextLevel = nextLevel;
	mSounds = sounds;
	mSubtitles = subtitles;
	CollisionComponent* cc = new CollisionComponent(this);
	cc->SetSize(1.0f, 1.0f, 1.0f);
}

void VOTrigger::OnUpdate(float deltaTime)
{
	AudioSystem* audio = mGame->GetAudio();
	if (mGame->GetPlayer()->GetComponent<HealthComponent>()->IsDead())
	{
		if (audio->GetSoundState(mPlayingSound) == SoundState::Playing)
		{
			audio->StopSound(mPlayingSound);
		}
	}
	else
	{
		if (!mInteracted)
		{
			if (GetComponent<CollisionComponent>()->Intersect(
					mGame->GetPlayer()->GetComponent<CollisionComponent>()))
			{
				mInteracted = true;
				PlaySoundSubtitle(audio);
			}
		}
		else if (audio->GetSoundState(mPlayingSound) == SoundState::Stopped)
		{
			if (mNextSound == mSounds.size())
			{
				mGame->GetPlayer()->GetHUD()->ShowSubtitle("");
				if (!mDoor.empty())
				{
					mGame->GetDoor(mDoor)->Open();
				}
				if (!mNextLevel.empty())
				{
					mGame->SetNextLevel(mNextLevel);
				}
				mState = ActorState::Destroy;
			}
			else
			{
				PlaySoundSubtitle(audio);
			}
		}
	}
}

void VOTrigger::OnProcessInput(const Uint8* keyState, Uint32 mouseButtons,
							   const Vector2& relativeMouse)
{
	if (keyState[SDL_SCANCODE_F])
	{
		if (!mFPressed)
		{
			mGame->GetAudio()->StopSound(mPlayingSound);
		}
	}
	mFPressed = keyState[SDL_SCANCODE_F];
}

void VOTrigger::PlaySoundSubtitle(AudioSystem* audio)
{
	mPlayingSound = audio->PlaySound(mSounds[mNextSound]);
	mGame->GetPlayer()->GetHUD()->ShowSubtitle(mSubtitles[mNextSound]);
	mNextSound++;
}