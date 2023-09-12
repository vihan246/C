#include "AudioSystem.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#include "Game.h"
#include "Actor.h"
#include "Player.h"
#include <filesystem>

// Create the AudioSystem with specified number of channels
// (Defaults to 8 channels)
AudioSystem::AudioSystem(Game* inGame, int numChannels)
{
	mGame = inGame;
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	Mix_AllocateChannels(numChannels);
	mChannels.resize((size_t)numChannels);
}

// Destroy the AudioSystem
AudioSystem::~AudioSystem()
{
	std::unordered_map<std::string, Mix_Chunk*>::iterator itr;
	for (itr = mSounds.begin(); itr != mSounds.end(); itr++)
	{
		Mix_FreeChunk(itr->second);
	}
	mSounds.clear();
	Mix_CloseAudio();
}

// Updates the status of all the active sounds every frame
void AudioSystem::Update(float deltaTime)
{
	for (size_t i = 0; i < mChannels.size(); i++)
	{
		if (mChannels.at(i) != 0)
		{
			if (Mix_Playing((int)i) == 0)
			{
				if (mHandleMap[mChannels.at(i)].mActor)
				{
					mActorMap[mHandleMap[mChannels.at(i)].mActor].erase(mChannels.at(i));
				}
				mHandleMap.erase(mChannels.at(i));
				mChannels.at(i) = 0;
			}
			else
			{
				if (mHandleMap[mChannels.at(i)].mActor)
				{
					int volume =
						CalculateVolume(mHandleMap[mChannels.at(i)].mActor, mGame->GetPlayer());
					Mix_Volume(mHandleMap[mChannels.at(i)].mChannel, volume);
				}
			}
		}
	}
}

// Plays the sound with the specified name and loops if looping is true
// Returns the SoundHandle which is used to perform any other actions on the
// sound when active
// NOTE: The soundName is without the "Assets/Sounds/" part of the file
//       For example, pass in "ChompLoop.wav" rather than
//       "Assets/Sounds/ChompLoop.wav".
SoundHandle AudioSystem::PlaySound(const std::string& soundName, bool looping, class Actor* actor,
								   bool stopOnActorRemove, int fadeTimeMS)
{
	Mix_Chunk* sound = GetSound(soundName);
	if (sound == nullptr)
	{
		SDL_Log("[AudioSystem] PlaySound couldn't find sound for %s", soundName.c_str());
		return 0;
	}
	mLastHandle++;
	int openChannel = -1;
	for (size_t i = 0; i < mChannels.size(); i++)
	{
		if (mChannels.at(i) == 0)
		{
			openChannel = (int)i;
			mChannels.at(i) = mLastHandle;
			break;
		}
	}
	if (openChannel == -1)
	{
		SoundHandle toOverwrite = -1;
		std::string soundNameToStop;
		std::map<SoundHandle, HandleInfo>::iterator itr;
		for (itr = mHandleMap.begin(); itr != mHandleMap.end(); itr++)
		{
			if (itr->second.mSoundName == soundName)
			{
				openChannel = itr->second.mChannel;
				soundNameToStop = itr->second.mSoundName;
				toOverwrite = itr->first;
				break;
			}
		}
		if (openChannel == -1)
		{
			for (itr = mHandleMap.begin(); itr != mHandleMap.end(); itr++)
			{
				if (!itr->second.mIsLooping)
				{
					openChannel = itr->second.mChannel;
					toOverwrite = itr->first;
					soundNameToStop = itr->second.mSoundName;
					break;
				}
			}
			if (openChannel == -1)
			{
				openChannel = mHandleMap.begin()->second.mChannel;
				toOverwrite = mHandleMap.begin()->first;
				soundNameToStop = mHandleMap.begin()->second.mSoundName;
			}
		}
		mHandleMap.erase(toOverwrite);
		SDL_Log("[AudioSystem] PlaySound ran out of channels playing %s! Stopping %s",
				soundName.c_str(), soundNameToStop.c_str());
	}
	HandleInfo info;
	info.mSoundName = soundName;
	info.mIsLooping = looping;
	info.mChannel = openChannel;
	info.mIsPaused = false;
	info.mActor = actor;
	info.mStopOnActorRemove = stopOnActorRemove;
	mHandleMap[mLastHandle] = info;
	if (fadeTimeMS > 0)
	{
		Mix_FadeInChannel(openChannel, sound, looping ? -1 : 0, fadeTimeMS);
	}
	else
	{
		Mix_PlayChannel(openChannel, sound, looping ? -1 : 0);
		int volume = CalculateVolume(actor, mGame->GetPlayer());
		Mix_Volume(openChannel, volume);
	}
	if (actor)
	{
		mActorMap[actor].emplace(mLastHandle);
	}
	return mLastHandle;
}

// Stops the sound if it is currently playing
void AudioSystem::StopSound(SoundHandle sound, int fadeTimeMS)
{
	if (mHandleMap.find(sound) == mHandleMap.end())
	{
		SDL_Log("[AudioSystem] StopSound couldn't find handle %u", sound);
	}
	else
	{
		if (fadeTimeMS > 0)
		{
			Mix_FadeOutChannel(mHandleMap[sound].mChannel, fadeTimeMS);
		}
		else
		{
			Mix_HaltChannel(mHandleMap[sound].mChannel);
			mChannels[mHandleMap[sound].mChannel] = 0;
			mHandleMap.erase(sound);
		}
	}
}

// Pauses the sound if it is currently playing
void AudioSystem::PauseSound(SoundHandle sound)
{
	if (mHandleMap.find(sound) == mHandleMap.end())
	{
		SDL_Log("[AudioSystem] PauseSound couldn't find handle %u", sound);
	}
	else if (!mHandleMap[sound].mIsPaused)
	{
		Mix_Pause(mHandleMap[sound].mChannel);
		mHandleMap[sound].mIsPaused = true;
	}
}

// Resumes the sound if it is currently paused
void AudioSystem::ResumeSound(SoundHandle sound)
{
	if (mHandleMap.find(sound) == mHandleMap.end())
	{
		SDL_Log("[AudioSystem] ResumeSound couldn't find handle %u", sound);
	}
	else if (mHandleMap[sound].mIsPaused)
	{
		Mix_Resume(mHandleMap[sound].mChannel);
		mHandleMap[sound].mIsPaused = false;
	}
}

// Returns the current state of the sound
SoundState AudioSystem::GetSoundState(SoundHandle sound)
{
	if (mHandleMap.find(sound) == mHandleMap.end())
	{
		return SoundState::Stopped;
	}
	else if (mHandleMap[sound].mIsPaused)
	{
		return SoundState::Paused;
	}
	else
	{
		return SoundState::Playing;
	}
}

// Stops all sounds on all channels
void AudioSystem::StopAllSounds()
{
	Mix_HaltChannel(-1);
	for (size_t i = 0; i < mChannels.size(); i++)
	{
		mChannels.at(i) = 0;
	}
	mHandleMap.clear();
}

// Cache all sounds under Assets/Sounds
void AudioSystem::CacheAllSounds()
{
#ifndef __clang_analyzer__
	std::error_code ec{};
	for (const auto& rootDirEntry : std::filesystem::directory_iterator{"Assets/Sounds", ec})
	{
		std::string extension = rootDirEntry.path().extension().string();
		if (extension == ".ogg" || extension == ".wav")
		{
			std::string fileName = rootDirEntry.path().stem().string();
			fileName += extension;
			CacheSound(fileName);
		}
	}
#endif
}

// Used to preload the sound data of a sound
// NOTE: The soundName is without the "Assets/Sounds/" part of the file
//       For example, pass in "ChompLoop.wav" rather than
//       "Assets/Sounds/ChompLoop.wav".
void AudioSystem::CacheSound(const std::string& soundName)
{
	GetSound(soundName);
}

// If the sound is already loaded, returns Mix_Chunk from the map.
// Otherwise, will attempt to load the file and save it in the map.
// Returns nullptr if sound is not found.
// NOTE: The soundName is without the "Assets/Sounds/" part of the file
//       For example, pass in "ChompLoop.wav" rather than
//       "Assets/Sounds/ChompLoop.wav".
Mix_Chunk* AudioSystem::GetSound(const std::string& soundName)
{
	std::string fileName = "Assets/Sounds/";
	fileName += soundName;

	Mix_Chunk* chunk = nullptr;
	auto iter = mSounds.find(fileName);
	if (iter != mSounds.end())
	{
		chunk = iter->second;
	}
	else
	{
		chunk = Mix_LoadWAV(fileName.c_str());
		if (!chunk)
		{
			SDL_Log("[AudioSystem] Failed to load sound file %s", fileName.c_str());
			return nullptr;
		}

		mSounds.emplace(fileName, chunk);
	}
	return chunk;
}

// Input for debugging purposes
void AudioSystem::ProcessInput(const Uint8* keyState)
{
	// Debugging code that outputs all active sounds on leading edge of period key
	if (keyState[SDL_SCANCODE_PERIOD] && !mLastDebugKey)
	{
		SDL_Log("[AudioSystem] Active Sounds:");
		for (size_t i = 0; i < mChannels.size(); i++)
		{
			if (mChannels[i] != 0)
			{
				auto iter = mHandleMap.find(mChannels[i]);
				if (iter != mHandleMap.end())
				{
					HandleInfo& hi = iter->second;
					SDL_Log("Channel %d: %u, %s, looping = %d, paused = %d",
							static_cast<unsigned>(i), mChannels[i], hi.mSoundName.c_str(),
							hi.mIsLooping, hi.mIsPaused);
				}
				else
				{
					SDL_Log("Channel %d: %u INVALID", static_cast<unsigned>(i), mChannels[i]);
				}
			}
		}
	}

	mLastDebugKey = keyState[SDL_SCANCODE_PERIOD];
}

void AudioSystem::RemoveActor(Actor* actor)
{
	if (mActorMap.find(actor) != mActorMap.end())
	{
		for (auto itr : mActorMap[actor])
		{
			if (mHandleMap.find(itr) != mHandleMap.end())
			{
				mHandleMap[itr].mActor = nullptr;
				if (mHandleMap[itr].mStopOnActorRemove && Mix_Playing(mHandleMap[itr].mChannel))
				{
					Mix_HaltChannel(mHandleMap[itr].mChannel);
				}
			}
		}
		mActorMap.erase(actor);
	}
}

int AudioSystem::CalculateVolume(Actor* actor, Actor* listener) const
{
	if (!actor || !listener)
	{
		return MAX_VOLUME;
	}
	Vector3 distanceVector = actor->GetWorldPosition() - listener->GetWorldPosition();
	float distance = distanceVector.Length();
	if (distance >= mMaxVolumeDist)
	{
		return MIN_VOLUME;
	}
	if (distance <= mMinVolumeDist)
	{
		return MAX_VOLUME;
	}

	float volumeRatio = 1 - ((distance - mMinVolumeDist) / (mMaxVolumeDist - mMinVolumeDist));
	int volume = (int)((float)MAX_VOLUME * volumeRatio);
	return volume;
}