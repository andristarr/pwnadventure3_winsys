#pragma once

#include "pch.h"
#include <vector>

#include "gameTypes.h"

class ModifyGame
{
public:
	static void ChangePlayerWalkSpeed(DWORD pPlayer, float desiredWalkSpeed);
	static void ChangePlayerJumpSpeed(DWORD pPlayer, float desiredJumpSpeed);
	static void TpPlayer(DWORD pPlayer, Vector3 coords);
	static Vector3 GetActorPosition(DWORD pthis);
	static char* GetActorDisplayName(DWORD pthis);
};
