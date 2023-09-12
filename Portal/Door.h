#pragma once
#include "Actor.h"
#include <string>

class Door : public Actor
{
public:
	Door(class Game* game, std::string inName);
	~Door();
	void Open();
	std::string GetName() { return mName; };
	bool IsOpen() const { return mOpen; };
	void OnUpdate(float deltaTime) override;

private:
	bool mOpen = false;
	std::string mName;
	Actor* mLeft = nullptr;
	Actor* mRight = nullptr;
};
