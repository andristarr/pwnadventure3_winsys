#include "pch.h"
#include "ModifyGame.h"
#include "gameTypes.h"

void ModifyGame::ChangePlayerWalkSpeed(DWORD pPlayer, float desiredWalkSpeed)
{
	DWORD pwsLocation = pPlayer + WalkSpeedInPlayer;

	*(float*)pwsLocation = desiredWalkSpeed;
}

void ModifyGame::ChangePlayerJumpSpeed(DWORD pPlayer, float desiredJumpSpeed)
{
	DWORD pjsLocation = pPlayer + JumpSpeedInPlayer;

	*(float*)pjsLocation = desiredJumpSpeed;
}

void ModifyGame::TpPlayer(DWORD pPlayer, Vector3 coords)
{
	HANDLE moduleHandle = GetModuleHandle("GameLogic.dll");

	DWORD funcAddress = (DWORD)moduleHandle + ActorSetPosition;

	PlayerSetPositionFn PlayerSetPosition = (PlayerSetPositionFn)funcAddress;

	PlayerSetPosition(pPlayer, coords);
}

Vector3 ModifyGame::GetActorPosition(DWORD pthis)
{
	HANDLE moduleHandle = GetModuleHandle("GameLogic.dll");

	DWORD funcAddress = (DWORD)moduleHandle + ActorGetPosition;

	ActorGetPositionFn ActorGetPosition = (ActorGetPositionFn)funcAddress;

	return ActorGetPosition(pthis);
}

char* ModifyGame::GetActorDisplayName(DWORD pthis)
{
	HANDLE moduleHandle = GetModuleHandle("GameLogic.dll");

	DWORD funcAddress = (DWORD)moduleHandle + ActorGetDisplayName;

	ActorGetDisplayNameFn ActorGetDisplayName = (ActorGetDisplayNameFn)funcAddress;

	return ActorGetDisplayName(pthis);
}
