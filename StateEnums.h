#ifndef _STATE_ENUMS
#define _STATE_ENUMS

enum StateEnum
{
	STATE_MAINMENU,
	STATE_GAME,
	STATE_WIN,
	STATE_LOSE,
	STATE_SERVERWAIT,
	STATE_SERVERCONNECTED,
	STATE_CONNECTING
};

enum SpawnClass
{
	CLASS_ACTOR,
	CLASS_MOUSE,
	CLASS_TANK,
	CLASS_GUITEXT
};

enum ServerCommand
{
	INITCLIENT,
	ADDUNIT
};

enum ClientCommand
{
	CONFIRMADD,
	UPDATEALLACTORS
};

#endif