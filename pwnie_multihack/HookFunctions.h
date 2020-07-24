#pragma once
#include <string>
#include <vector>
#include <sstream>
#include "ModifyGame.h"


#include <iostream>
#include <fstream>
#include <set>

std::string moduleName = "GameLogic.dll";

PlayerTickFn oPlayerTick;
PlayerChatFn oPlayerChat;
ClientWorldTickFn oClientWorldTick;

DWORD pPlayer = 0x0;
DWORD pClientWorld = 0x0;

bool frozen = false;
Vector3 freezePosition;

void __fastcall hookedPlayerTick(DWORD pthis, DWORD _edx, float passForward)
{
	pPlayer = pthis;

	if (frozen)
	{
		ModifyGame::TpPlayer(pPlayer, freezePosition);
	}

	return oPlayerTick(pthis, passForward);
}

void __fastcall hookedClientWorldTick(DWORD pthis, DWORD _edx, float passForward)
{
	pClientWorld = pthis;

	return oClientWorldTick(pthis, passForward);
}

void handleChangeWalkSpeed(std::string& wmsg)
{
	std::stringstream wmsg_ss(wmsg);
	std::string segment;
	std::vector<std::string> seglist;

	while (std::getline(wmsg_ss, segment, ' '))
	{
		seglist.push_back(segment);
	}

	try
	{
		float desiredWalkSpeed = std::stof(seglist[1]);
		ModifyGame::ChangePlayerWalkSpeed(pPlayer, desiredWalkSpeed);
	}
	catch (std::invalid_argument)
	{
	}
}

void handleChangeJumpSpeed(std::string& wmsg)
{
	std::stringstream wmsg_ss(wmsg);
	std::string segment;
	std::vector<std::string> seglist;

	while (std::getline(wmsg_ss, segment, ' '))
	{
		seglist.push_back(segment);
	}

	try
	{
		float desiredJumpSpeed = std::stof(seglist[1]);
		ModifyGame::ChangePlayerJumpSpeed(pPlayer, desiredJumpSpeed);
	}
	catch (std::invalid_argument)
	{
	}
}

void handleTp(std::string& wmsg)
{
	std::stringstream wmsg_ss(wmsg);
	std::string segment;
	std::vector<std::string> seglist;

	while (std::getline(wmsg_ss, segment, ' '))
	{
		seglist.push_back(segment);
	}

	try
	{
		Vector3 coords;

		coords.x = std::stof(seglist[1]);
		coords.y = std::stof(seglist[2]);
		coords.z = std::stof(seglist[3]);

		std::ofstream myf;
		myf.open("debug.txt");
		myf << coords.x << " " << coords.y << " " << coords.z << "\n";
		myf.close();

		ModifyGame::TpPlayer(pPlayer, coords);
	}
	catch (std::invalid_argument)
	{
	}
}

void handleTpZ(std::string& wmsg)
{
	std::stringstream wmsg_ss(wmsg);
	std::string segment;
	std::vector<std::string> seglist;

	while (std::getline(wmsg_ss, segment, ' '))
	{
		seglist.push_back(segment);
	}

	try
	{
		Vector3 coords;

		coords = ModifyGame::GetActorPosition(pPlayer);

		coords.z = std::stof(seglist[1]);

		ModifyGame::TpPlayer(pPlayer, coords);
	}
	catch (std::invalid_argument)
	{
	}
}

void handleFreeze(bool freeze)
{
	if (freeze)
	{
		freezePosition = ModifyGame::GetActorPosition(pPlayer);
		frozen = true;
	}
	else
	{
		frozen = false;
	}
}

void handleGetEggs()
{
	ClientWorld cw = *(ClientWorld*)pClientWorld;

	std::ofstream fEggs;
	fEggs.open("eggscoords.txt");
	
	for(auto elem : cw.m_actors)
	{
		auto coords = ModifyGame::GetActorPosition((DWORD)elem.m_object);
		fEggs << "X: " << coords.x << " Y: " << coords.y << " Z: " << coords.z << " Name: " << ModifyGame::GetActorDisplayName((DWORD)elem.m_object) << "\n";
	}

	fEggs.close();
}

void __fastcall hookedPlayerChat(DWORD pthis, DWORD _edx, char* msg)
{
	std::string wmsg(msg);

	if (wmsg.length() >= 3 && wmsg.substr(0, 3)._Equal("ws "))
	{
		handleChangeWalkSpeed(wmsg);
		return;
	}
	if (wmsg.length() >= 3 && wmsg.substr(0, 3)._Equal("js "))
	{
		handleChangeJumpSpeed(wmsg);
		return;
	}

	if (wmsg.length() >= 3 && wmsg.substr(0, 3)._Equal("tp "))
	{
		handleTp(wmsg);
		return;
	}

	if (wmsg.length() >= 3 && wmsg.substr(0, 3)._Equal("tpz"))
	{
		handleTpZ(wmsg);
		return;
	}

	if (wmsg.length() >= 3 && wmsg.substr(0, 3)._Equal("fre"))
	{
		handleFreeze(true);
		return;
	}

	if (wmsg.length() >= 3 && wmsg.substr(0, 3)._Equal("unf"))
	{
		handleFreeze(false);
		return;
	}

	if (wmsg.length() >= 3 && wmsg.substr(0, 3)._Equal("egg"))
	{
		handleGetEggs();
		return;
	}

	return oPlayerChat(pthis, msg);
}
