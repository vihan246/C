#include "Prop.h"
#include "CollisionComponent.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"

Prop::Prop(Game* game, bool inAlpha, bool inCollision, std::string inMesh)
: Actor(game)
{
	MeshComponent* mc = new MeshComponent(this, inAlpha);
	Mesh* mesh = GetGame()->GetRenderer()->GetMesh(inMesh);
	mc->SetMesh(mesh);
	if (inCollision)
	{
		CollisionComponent* cc = new CollisionComponent(this);
		cc->SetSize(mesh->GetWidth(), mesh->GetHeight(), mesh->GetDepth());
		GetGame()->AddCollider(this);
	}
}

Prop::~Prop()
{
	GetGame()->RemoveCollider(this);
}