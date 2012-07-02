#ifndef _TEMPORALDEF
#define _TEMPORALDEF

// Windows Header Files:
#include <winsock2.h>
#define _WINSOCKDEF

#include <windows.h>

// C RunTime Header Files:
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>

#pragma comment(lib, "d2d1")

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <math.h>
#include <time.h>

using namespace std;


template<class Interface>
inline void SafeRelease(
    Interface **ppInterfaceToRelease
    )
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();

        (*ppInterfaceToRelease) = NULL;
    }
}


#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif



#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

#include "StateEnums.h"
#include "ActorClasses.h"
#include "TemporalSocket.h"
#include <vector>

class Temporal;
class TemporalSocket;

class Temporal
{
public:
    Temporal();
    ~Temporal();

	/*All The actors in the given Level*/
	vector<Actor> AllActors;

	/*Flag for ready to render!*/
	bool bReadyToRender;

    // Register the window class and call methods for instantiating drawing resources
    HRESULT Initialize();

    // Process and dispatch messages
    void RunMessageLoop();

	//Spawn Actors
	int Spawn(SpawnClass pClass, float pX, float pY, ActorScale pScale, Teams pTeam);

	//the Socket Connection
	TemporalSocket* ConnSocket;

	//Process Message Sent from server to client
	void ProcessMessage(char* pServerMessage);

	//Process Message sent from client to server
	ClientCommand ServerProcessMessage(char* pClientMessage);

	//Registers a new client to the server
	void RegisterNewClient(char* ClientName);

private:

    // Initialize device-independent resources.
    HRESULT CreateDeviceIndependentResources();

    // Initialize device-dependent resources.
    HRESULT CreateDeviceResources();

    // Release device-dependent resource.
    void DiscardDeviceResources();

    // Draw content.
    HRESULT OnRender();

    // Resize the render target.
    void OnResize(UINT width, UINT height);

    // The windows procedure.
    static LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	bool DrawMenu();
	
	bool DrawGame(double* DT);

	bool DrawWin();

	bool DrawLose();

	bool DrawWaiting(char* Message);

	bool DrawServerScreen();

	StateEnum CurrentState;

	double DeltaTime();

	clock_t LastTime;

	void initState(StateEnum pState);

	//Safely delete an actor from AllActors
	void DeleteActor(int index);

	//Inits the mouse Actor object in AllObjects[0]
	void InitMouse();

	//The current WIndow Lengths and heights
	UINT WindowWidth, WindowHeight;

	//Tell current Temporal Class to change states
	void GoToState(StateEnum pState);

	//Send Server the clientName and download the current AllActors array
	char* SendClientInfo();

	


private:
	HWND m_hwnd;
	ID2D1Factory* m_pDirect2dFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	ID2D1SolidColorBrush* m_pLightSlateGrayBrush;
	ID2D1SolidColorBrush* m_pCornflowerBlueBrush;

};

#endif