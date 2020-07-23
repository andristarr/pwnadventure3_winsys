#include "pch.h"
#include "HookHelper.h"
#include "gameTypes.h"
#include "HookFunctions.h"
#include "Detours.h"
#include <string>

bool HookHelper::HookFunctions()
{
	hookPlayerTick();
	hookPlayerChat();
	hookClientWorldTick();
	
	return true;
}

bool HookHelper::hookPlayerTick()
{
	HMODULE moduleHandle = GetModuleHandle(moduleName.c_str());

	DWORD funcAddress = (DWORD)moduleHandle + GameFuncs::PlayerTick;
	
	oPlayerTick = (PlayerTickFn)Detours::X86::DetourFunction((uintptr_t)funcAddress, (uintptr_t)hookedPlayerTick);

	return true;
}

bool HookHelper::hookPlayerChat()
{
	HMODULE moduleHandle = GetModuleHandle(moduleName.c_str());

	DWORD funcAddress = (DWORD)moduleHandle + GameFuncs::PlayerChat;

	oPlayerChat = (PlayerChatFn)Detours::X86::DetourFunction((uintptr_t)funcAddress, (uintptr_t)hookedPlayerChat);

	return true;
}

bool HookHelper::hookClientWorldTick()
{
	HMODULE moduleHandle = GetModuleHandle(moduleName.c_str());

	DWORD funcAddress = (DWORD)moduleHandle + GameFuncs::ClientWorldTick;

	oClientWorldTick = (ClientWorldTickFn)Detours::X86::DetourFunction((uintptr_t)funcAddress, (uintptr_t)hookedClientWorldTick);

	return true;
}
