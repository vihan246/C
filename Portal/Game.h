#pragma once
#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#include <unordered_map>
#include <string>
#include <vector>
#include "Math.h"
#include "AudioSystem.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define emscripten_cancel_main_loop()
#endif

class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void EmRunIteration()
	{
		if (!mIsRunning)
		{
			emscripten_cancel_main_loop();
		}
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
	void Shutdown();

	void AddActor(class Actor* actor);
	void RemoveActor(class Actor* actor);

	AudioSystem* GetAudio() { return mAudio; }

	class Renderer* GetRenderer() { return mRenderer; }
	class Player* GetPlayer() { return mPlayer; };
	void SetPlayer(class Player* player) { mPlayer = player; }

	const float WINDOW_WIDTH = 1024.0f;
	const float WINDOW_HEIGHT = 768.0f;

	static const int FORWARD_SPAWN_DIST = 500;

	static constexpr float hDist = 60.0f;
	static constexpr float targetDist = 50.0f;
	static constexpr float vDist = 70.0f;

	static constexpr float playerScale = 0.75f;
	static const int MUSIC_FADE_TIME = 250;

	SoundHandle GetGameMusicHandle() const { return mGameMusicHandle; }
	void SetGameMusicHandle(SoundHandle newState) { mGameMusicHandle = newState; }

	void AddCollider(class Actor* actor) { mColliders.push_back(actor); }
	void RemoveCollider(class Actor* actor);
	std::vector<class Actor*>& GetColliders() { return mColliders; }

	class Portal* GetOrangePortal() { return mOrangePortal; }
	class Portal* GetBluePortal() { return mBluePortal; }
	void SetOrangePortal(class Portal* portal) { mOrangePortal = portal; }
	void SetBluePortal(class Portal* portal) { mBluePortal = portal; }

	void Reload() { mNextLevel = mCurrentLevel; }
	void SetNextLevel(std::string nextLevel) { mNextLevel = nextLevel; }
	void AddDoor(class Door* door);
	void RemoveDoor(class Door* door);
	class Door* GetDoor(std::string inName);

	std::vector<class Actor*> GetActors() const { return mActors; }

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void LoadData();
	void UnloadData();

	// All the actors in the game
	std::vector<class Actor*> mActors;

	class Renderer* mRenderer = nullptr;
	AudioSystem* mAudio = nullptr;

	Uint32 mTicksCount = 0;
	bool mIsRunning;

	class Player* mPlayer;

	SoundHandle mGameMusicHandle;

	static constexpr float mFovY = 1.22f;
	static constexpr float mPerspectiveNear = 10.0f;
	static constexpr float mPerpectiveFar = 10000.0f;

	std::vector<class Actor*> mColliders;

	class Portal* mOrangePortal;
	class Portal* mBluePortal;

	std::string mCurrentLevel;
	class InputReplay* mInputReplay;

	std::string mNextLevel;
	bool mReloadPressed = false;

	std::unordered_map<std::string, class Door*> mDoors;
};
