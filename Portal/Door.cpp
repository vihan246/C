#include "Door.h"
#include "Game.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "Mesh.h"
#include "Renderer.h"

Door::Door(Game* game, std::string inName)
: Actor(game)
{
	MeshComponent* mc = new MeshComponent(this);
	Mesh* mesh = game->GetRenderer()->GetMesh("Assets/Meshes/DoorFrame.gpmesh");
	mc->SetMesh(mesh);
	CollisionComponent* cc = new CollisionComponent(this);
	cc->SetSize(mesh->GetWidth(), mesh->GetHeight(), mesh->GetDepth());
	mName = inName;

	mLeft = new Actor(game, this);
	MeshComponent* mcLeft = new MeshComponent(mLeft);
	mcLeft->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshes/DoorLeft.gpmesh"));
	mRight = new Actor(game, this);
	MeshComponent* mcRight = new MeshComponent(mRight);
	mcRight->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshes/DoorRight.gpmesh"));
	game->AddCollider(this);
	game->AddDoor(this);
}

Door::~Door()
{
	if (!mOpen)
	{
		mGame->RemoveCollider(this);
	}
	mGame->RemoveDoor(this);
}

void Door::Open()
{
	if (!mOpen)
	{
		GetGame()->GetAudio()->PlaySound("DoorOpen.ogg", false, this);
		mOpen = true;
		mGame->RemoveCollider(this);
	}
}

void Door::OnUpdate(float deltaTime)
{
	if (mOpen)
	{
		Vector3 mLeftPos = mLeft->GetPosition();
		Vector3 mRightPos = mRight->GetPosition();
		mLeftPos.y = Math::Lerp(mLeftPos.y, -100.0f, 0.1f);
		mRightPos.y = Math::Lerp(mRightPos.y, 100.0f, 0.1f);
		mLeft->SetPosition(mLeftPos);
		mRight->SetPosition(mRightPos);
	}
}