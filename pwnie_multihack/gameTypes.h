#pragma once
#include "pch.h"

#include <set>
#include <map>
#include <set>
#include <map>
#include <functional>

enum GameFuncs
{
	PlayerTick = 0x50730,
	PlayerChat = 0x551A0,
	ActorSetPosition = 0x1C80,
	ActorGetPosition = 0x16F0,
	ClientWorldTick = 0xCAE0,
	ActorGetDisplayName = 0x17C0,
};

enum GameOffsets
{
	WalkSpeedInPlayer = 0x190,
	JumpSpeedInPlayer = 0x194,
};

struct Vector3
{
	float x;
	float y;
	float z;
};

typedef void (__thiscall* PlayerTickFn)(DWORD pthis, float passForward);
typedef void (__thiscall* PlayerChatFn)(DWORD pthis, char* msg);
typedef void (__thiscall* PlayerSetPositionFn)(DWORD pthis, Vector3& coords);
typedef Vector3 (__thiscall* ActorGetPositionFn)(DWORD pthis);
typedef void (__thiscall* ClientWorldTickFn)(DWORD pthis, float passForward);
typedef char* (__thiscall* ActorGetDisplayNameFn)(DWORD pthis);


#define __int8 char
#define __int16 short
#define __int32 int
#define __int64 long long

struct IPlayer
{
};

struct ILocalPlayer
{
};

struct IActor
{
};

template <class T>
class ActorRef
{
public:
	IActor* m_object;
};

struct AIZone
{
};

struct WorldVtbl
{
};

struct World
{
	WorldVtbl* vfptr;
	std::set<ActorRef<IPlayer>, std::less<ActorRef<IPlayer> >, std::allocator<ActorRef<IPlayer> > > m_players;
	std::set<ActorRef<IActor>, std::less<ActorRef<IActor> >, std::allocator<ActorRef<IActor> > > m_actors;
	std::map<unsigned int, ActorRef<IActor>, std::less<unsigned int>, std::allocator<std::pair<unsigned int const, ActorRef<IActor> > > > m_actorsById;
	ILocalPlayer* m_localPlayer;
	unsigned int m_nextId;
	std::map<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, AIZone*, std::less<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<std::basic_string<char, std::char_traits<char>, std::allocator<char> > const, AIZone*> > > m_aiZones;

};

struct ClientWorld : World
{
	ActorRef<IPlayer> m_activePlayer;
	float m_timeUntilNextNetTick;
};
