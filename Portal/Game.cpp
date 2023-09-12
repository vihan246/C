//
//  Game.cpp
//  Game-mac
//
//  Created by Sanjay Madhav on 5/31/17.
//  Copyright © 2017 Sanjay Madhav. All rights reserved.
//

#include "Game.h"
#include <algorithm>
#include "Actor.h"
#include <fstream>
#include "Renderer.h"
#include "Random.h"
#include "Player.h"
#include "MeshComponent.h"
#include "LevelLoader.h"
#include "Portal.h"
#include "InputReplay.h"
#include "Door.h"
#include <SDL2/SDL_ttf.h>

Game::Game()
: mIsRunning(true)
{
	mPlayer = nullptr;
	mGameMusicHandle = 0;
	mOrangePortal = nullptr;
	mBluePortal = nullptr;
	mInputReplay = nullptr;
}

bool Game::Initialize()
{
	Random::Init();

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	mRenderer = new Renderer(this);
	if (!mRenderer->Initialize(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT))
	{
		SDL_Log("Unable to intialize renderer");
		return false;
	}

	// On Mac, tell SDL that CTRL+Click should generate a Right Click event
	SDL_SetHint(SDL_HINT_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK, "1");
	// Enable relative mouse mode
	SDL_SetRelativeMouseMode(SDL_TRUE);
	// Clear any saved values
	SDL_GetRelativeMouseState(nullptr, nullptr);

	mAudio = new AudioSystem(this, 32);
	TTF_Init();
	LoadData();

	mTicksCount = SDL_GetTicks();

	mInputReplay = new InputReplay(this);

	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
	if (state[SDL_SCANCODE_P])
	{
		mInputReplay->StartPlayback(mCurrentLevel);
	}
	if (state[SDL_SCANCODE_F5])
	{
		if (!mReloadPressed)
		{
			mNextLevel = mCurrentLevel;
		}
		mReloadPressed = true;
	}
	else
	{
		mReloadPressed = false;
	}
	int x = 0;
	int y = 0;
	Uint32 mouseButtons = SDL_GetRelativeMouseState(&x, &y);
	Vector2 relativeMouse(x, y);

	mInputReplay->InputPlayback(state, mouseButtons, relativeMouse);

	for (auto actor : mActors)
	{
		actor->ProcessInput(state, mouseButtons, relativeMouse);
	}

	mAudio->ProcessInput(state);
}

void Game::UpdateGame()
{
	// Compute delta time
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	float deltaTime = 0.016f;
	mTicksCount = SDL_GetTicks();

	mAudio->Update(deltaTime);
	mInputReplay->Update(deltaTime);
	// Make copy of actor vector
	// (iterate over this in case new actors are created)
	std::vector<Actor*> copy = mActors;
	// Update all actors
	for (auto actor : copy)
	{
		actor->Update(deltaTime);
	}

	// Add any actors to destroy to a temp vector
	std::vector<Actor*> destroyActors;
	for (auto actor : mActors)
	{
		if (actor->GetState() == ActorState::Destroy)
		{
			destroyActors.emplace_back(actor);
		}
	}

	// Delete the destroyed actors (which will
	// remove them from mActors)
	for (auto actor : destroyActors)
	{
		delete actor;
	}

	if (!mNextLevel.empty())
	{
		UnloadData();
		mInputReplay->StopPlayback();
		mAudio->StopAllSounds();
		mBluePortal = nullptr;
		mOrangePortal = nullptr;
		mCurrentLevel = mNextLevel;
		LevelLoader::Load(this, mCurrentLevel);
		mNextLevel = "";
	}
}

void Game::GenerateOutput()
{
	// TODO: tell renderer to draw
	mRenderer->Draw();
}

void Game::LoadData()
{
	Matrix4 projectionMatrix = Matrix4::CreatePerspectiveFOV(
		mFovY, Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT, mPerspectiveNear, mPerpectiveFar);
	mRenderer->SetProjectionMatrix(projectionMatrix);
	mAudio->CacheAllSounds();

	mCurrentLevel = "Assets/Level01.json";
	LevelLoader::Load(this, mCurrentLevel);
}

void Game::UnloadData()
{
	// Delete actors
	// Because ~Actor calls RemoveActor, have to use a different style loop
	while (!mActors.empty())
	{
		delete mActors.back();
	}
}

void Game::Shutdown()
{
	UnloadData();
	delete mAudio;
	mRenderer->Shutdown();
	delete mRenderer;
	delete mInputReplay;
	TTF_Quit();
	SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
	mActors.emplace_back(actor);
}

void Game::RemoveActor(Actor* actor)
{
	auto iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		mActors.erase(iter);
	}
}

void Game::RemoveCollider(Actor* actor)
{
	auto iter = std::find(mColliders.begin(), mColliders.end(), actor);
	if (iter != mColliders.end())
	{
		mColliders.erase(iter);
	}
}

void Game::AddDoor(Door* door)
{
	mDoors.emplace(door->GetName(), door);
}

void Game::RemoveDoor(Door* door)
{
	mDoors.erase(door->GetName());
}

Door* Game::GetDoor(std::string inName)
{
	if (mDoors.find(inName) != mDoors.end())
	{
		return mDoors[inName];
	}
	return nullptr;
}