#pragma once
#include <map>
#include <vector>
#include <string>
#include <SDL2/SDL_scancode.h>
#include "Math.h"

class InputReplay
{
public:
	InputReplay(class Game* game);

	void StartRecording(const std::string& levelName);
	void StopRecording();

	void StartPlayback(const std::string& levelName);
	void StopPlayback();

	void RecordInput(const Uint8* keyState, Uint32 mouseButtons, const Vector2& relativeMouse);
	void Update(float deltaTime);

	void InputPlayback(const Uint8*& keyState, Uint32& mouseButtons, Vector2& relativeMouse);

private:
	std::string mLevelName;
	std::map<SDL_Scancode, bool> mKeyStates;
	Uint32 mLastMouseButtons = 0;

	struct InputEvent
	{
		float mTimestamp = -1.0f;
		std::map<SDL_Scancode, bool> mKeyChanges;
		Uint32 mMouseButtons = 0;
		Vector2 mRelativeMouse;
	};
	std::vector<InputEvent> mEvents;

	Uint8 mPlaybackKeys[SDL_NUM_SCANCODES];

	bool mIsRecording = false;
	bool mIsInPlayback = false;

	float mLastTimestamp = 0.0f;
	int mCurrentPlaybackIndex = -1;

	class Game* mGame = nullptr;
};
