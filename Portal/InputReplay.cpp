#include "InputReplay.h"
#include <rapidjson/document.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>
#include <fstream>
#include <sstream>
#include "SDL2/SDL_log.h"
#include "Game.h"
#include "Player.h"
#include "CameraComponent.h"

namespace
{
	std::string ConvertLevelToReplay(const std::string& levelName)
	{
		size_t dotLoc = levelName.find_first_of('/');
		std::string firstHalf = levelName.substr(0, dotLoc + 1);
		std::string secondHalf = levelName.substr(dotLoc);
		return firstHalf + "Replays/" + secondHalf;
	}
} // namespace

InputReplay::InputReplay(Game* game)
{
	mGame = game;
	mKeyStates.emplace(SDL_SCANCODE_W, false);
	mKeyStates.emplace(SDL_SCANCODE_A, false);
	mKeyStates.emplace(SDL_SCANCODE_S, false);
	mKeyStates.emplace(SDL_SCANCODE_D, false);
	mKeyStates.emplace(SDL_SCANCODE_R, false);
	mKeyStates.emplace(SDL_SCANCODE_F, false);
	mKeyStates.emplace(SDL_SCANCODE_F5, false);
	mKeyStates.emplace(SDL_SCANCODE_SPACE, false);

	std::memset(mPlaybackKeys, 0, sizeof(Uint8) * SDL_NUM_SCANCODES);
}

void InputReplay::StartRecording(const std::string& levelName)
{
	if (!mIsRecording && !mIsInPlayback)
	{
		mIsRecording = true;
		mLastTimestamp = 0.0f;
		mEvents.clear();
		mLevelName = levelName;
	}
}

void InputReplay::StopRecording()
{
	if (mIsRecording)
	{
		mIsRecording = false;

		// Write out the JSON
		rapidjson::Document doc;
		auto& allocator = doc.GetAllocator();
		doc.SetArray();
		for (const auto& event : mEvents)
		{
			rapidjson::Value entry;
			entry.SetObject();
			entry.AddMember("t", event.mTimestamp, allocator);

			rapidjson::Value mouse;
			mouse.SetObject();
			mouse.AddMember("x", event.mRelativeMouse.x, allocator);
			mouse.AddMember("y", event.mRelativeMouse.y, allocator);
			mouse.AddMember("b", event.mMouseButtons, allocator);
			entry.AddMember("m", mouse, allocator);

			rapidjson::Value keysV;
			keysV.SetArray();
			for (const auto& keyPair : event.mKeyChanges)
			{
				rapidjson::Value keyV;
				keyV.SetObject();
				keyV.AddMember("k", static_cast<int>(keyPair.first), allocator);
				keyV.AddMember("v", keyPair.second, allocator);
				keysV.PushBack(keyV, allocator);
			}
			entry.AddMember("k", keysV, allocator);
			doc.PushBack(entry, allocator);
		}

		std::string replayFile = ConvertLevelToReplay(mLevelName);
		std::ofstream file(replayFile);
		rapidjson::OStreamWrapper osw(file);

		rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
		doc.Accept(writer);
	}
}

void InputReplay::StartPlayback(const std::string& levelName)
{
	if (!mIsInPlayback && !mIsRecording)
	{
		mGame->GetPlayer()->SetPosition(mGame->GetPlayer()->GetInitialPosition());
		mGame->GetPlayer()->SetRotation(0.0f);
		mGame->GetPlayer()->GetComponent<CameraComponent>()->ResetPitchAngle();
		mIsInPlayback = true;
		mLastTimestamp = 0.0f;
		std::memset(mPlaybackKeys, 0, sizeof(Uint8) * SDL_NUM_SCANCODES);
		mEvents.clear();
		mCurrentPlaybackIndex = -1;

		// Read in the JSON
		std::string replayFile = ConvertLevelToReplay(levelName);
		std::ifstream file(replayFile);
		if (!file.is_open())
		{
			SDL_Log("Assets/Replay.json file not found");
			return;
		}

		std::stringstream fileStream;
		fileStream << file.rdbuf();
		std::string contents = fileStream.str();
		rapidjson::StringStream jsonStr(contents.c_str());
		rapidjson::Document doc;
		doc.ParseStream(jsonStr);

		if (!doc.IsArray())
		{
			SDL_Log("Assets/Replay.json file is invalid");
			return;
		}

		mEvents.reserve(doc.Size());
		for (rapidjson::SizeType i = 0; i < doc.Size(); i++)
		{
			const rapidjson::Value& iter = doc[i];

			InputEvent event;
			event.mTimestamp = iter["t"].GetFloat();
			event.mRelativeMouse.x = iter["m"]["x"].GetFloat();
			event.mRelativeMouse.y = iter["m"]["y"].GetFloat();
			event.mMouseButtons = iter["m"]["b"].GetUint();

			const rapidjson::Value& keys = iter["k"];
			for (rapidjson::SizeType j = 0; j < keys.Size(); j++)
			{
				event.mKeyChanges.emplace(static_cast<SDL_Scancode>(keys[j]["k"].GetInt()),
										  keys[j]["v"].GetBool());
			}

			mEvents.emplace_back(event);
		}
	}
}

void InputReplay::StopPlayback()
{
	if (mIsInPlayback)
	{
		mIsInPlayback = false;
	}
}

void InputReplay::RecordInput(const Uint8* keyState, Uint32 mouseButtons,
							  const Vector2& relativeMouse)
{
	if (mIsRecording)
	{
		InputEvent event;
		for (auto& key : mKeyStates)
		{
			if (static_cast<bool>(keyState[key.first]) != key.second)
			{
				event.mTimestamp = mLastTimestamp;
				event.mKeyChanges.emplace(key.first, keyState[key.first]);
				key.second = keyState[key.first];
			}
		}

		if (!Math::NearZero(relativeMouse.Length()))
		{
			event.mTimestamp = mLastTimestamp;
			event.mRelativeMouse = relativeMouse;
		}

		if (mouseButtons != mLastMouseButtons)
		{
			event.mTimestamp = mLastTimestamp;
			event.mMouseButtons = mouseButtons;
		}

		if (event.mTimestamp >= 0.0f)
		{
			mEvents.emplace_back(event);
		}
	}
}

void InputReplay::Update(float deltaTime)
{
	if (mIsRecording || mIsInPlayback)
	{
		mLastTimestamp += deltaTime;
	}
}

void InputReplay::InputPlayback(const Uint8*& keyState, Uint32& mouseButtons,
								Vector2& relativeMouse)
{
	if (mIsInPlayback)
	{
		keyState = mPlaybackKeys;
		relativeMouse = Vector2::Zero;
		mouseButtons = 0;
		while (static_cast<size_t>(mCurrentPlaybackIndex + 1) < mEvents.size() &&
			   mLastTimestamp >= mEvents[mCurrentPlaybackIndex + 1].mTimestamp)
		{
			mCurrentPlaybackIndex++;
			relativeMouse += mEvents[mCurrentPlaybackIndex].mRelativeMouse;
			mouseButtons = mEvents[mCurrentPlaybackIndex].mMouseButtons;

			for (const auto& key : mEvents[mCurrentPlaybackIndex].mKeyChanges)
			{
				mPlaybackKeys[key.first] = key.second;
			}
		}

		if (static_cast<size_t>(mCurrentPlaybackIndex + 1) == mEvents.size())
		{
			mIsInPlayback = false;
		}
	}
}
