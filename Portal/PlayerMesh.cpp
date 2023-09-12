#include "PlayerMesh.h"
#include "Game.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "Player.h"
#include "CameraComponent.h"

PlayerMesh::PlayerMesh(Game* game)
: Actor(game)
{
	MeshComponent* mc = new MeshComponent(this);
	mc->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshes/PortalGun.gpmesh"));
	SetScale(PLAYER_MESH_SCALE);
}

void PlayerMesh::OnUpdate(float deltaTime)
{
	Vector3 newPosition = GetGame()->GetRenderer()->Unproject(UNPROJECT_INPUT);
	SetPosition(newPosition);
	Player* player = GetGame()->GetPlayer();
	float pitchAngle = player->GetComponent<CameraComponent>()->GetPitchAngle();
	Quaternion quatPitch = Quaternion(Vector3::UnitY, pitchAngle);

	float yawAngle = player->GetRotation();
	Quaternion quatYaw = Quaternion(Vector3::UnitZ, yawAngle);

	SetQuat(Quaternion::Concatenate(quatPitch, quatYaw));
}