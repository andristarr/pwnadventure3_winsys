#pragma once
class HookHelper
{
public:
	static bool HookFunctions();

private:
	static bool hookPlayerTick();
	static bool hookPlayerChat();
	static bool hookClientWorldTick();
};

