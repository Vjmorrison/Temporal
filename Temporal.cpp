#include "Temporal.h"

///Constructor
Temporal::Temporal() :
    m_hwnd(NULL),
    m_pDirect2dFactory(NULL),
    m_pRenderTarget(NULL),
    m_pLightSlateGrayBrush(NULL),
    m_pCornflowerBlueBrush(NULL)
{
	bReadyToRender = false;
	CurrentState = STATE_MAINMENU;
	LastTime = clock();

	WindowWidth = 0;
	WindowHeight = 0;

	ConnSocket = new TemporalSocket(this);
}

///Destructor
Temporal::~Temporal()
{
    SafeRelease(&m_pDirect2dFactory);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pLightSlateGrayBrush);
    SafeRelease(&m_pCornflowerBlueBrush);

}

///
void Temporal::RunMessageLoop()
{
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

		OnRender();
    }
}

///INIT the main things
HRESULT Temporal::Initialize()
{
	HRESULT hr;

	initState(CurrentState);

    // Initialize device-indpendent resources, such
    // as the Direct2D factory.
    hr = CreateDeviceIndependentResources();

    if (SUCCEEDED(hr))
    {
        // Register the window class.
        WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
        wcex.style         = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc   = Temporal::WndProc;
        wcex.cbClsExtra    = 0;
        wcex.cbWndExtra    = sizeof(LONG_PTR);
        wcex.hInstance     = HINST_THISCOMPONENT;
        wcex.hbrBackground = NULL;
        wcex.lpszMenuName  = NULL;
        wcex.hCursor       = LoadCursor(NULL, IDI_APPLICATION);
        wcex.lpszClassName = (LPCSTR)L"D2DDemoApp";

        RegisterClassEx(&wcex);


        // Because the CreateWindow function takes its size in pixels,
        // obtain the system DPI and use it to scale the window size.
        FLOAT dpiX, dpiY;

        // The factory returns the current system DPI. This is also the value it will use
        // to create its own windows.
        m_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);


        // Create the window.
        m_hwnd = CreateWindow(
            (LPCSTR)L"D2DDemoApp",
            (LPCSTR)L"Direct2D Demo App",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            static_cast<UINT>(ceil(640.f * dpiX / 96.f)),
            static_cast<UINT>(ceil(480.f * dpiY / 96.f)),
            NULL,
            NULL,
            HINST_THISCOMPONENT,
            this
            );
        hr = m_hwnd ? S_OK : E_FAIL;
        if (SUCCEEDED(hr))
        {
            ShowWindow(m_hwnd, SW_SHOWNORMAL);
            UpdateWindow(m_hwnd);
        }
    }

    return hr;
}

int WINAPI WinMain(
    HINSTANCE /* hInstance */,
    HINSTANCE /* hPrevInstance */,
    LPSTR /* lpCmdLine */,
    int /* nCmdShow */
    )
{
    // Use HeapSetInformation to specify that the process should
    // terminate if the heap manager detects an error in any heap used
    // by the process.
    // The return value is ignored, because we want to continue running in the
    // unlikely event that HeapSetInformation fails.
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (SUCCEEDED(CoInitialize(NULL)))
    {
        {
            Temporal app;

            if (SUCCEEDED(app.Initialize()))
            {
                app.RunMessageLoop();
            }
        }
        CoUninitialize();
    }

    return 0;
}


/*Implement the Temporal::CreateDeviceIndependentResources method. In the method, create an ID2D1Factory, a device-independent resource, 
for creating other Direct2D resources. Use the m_pDirect2DdFactory class member to store the factory. */
HRESULT Temporal::CreateDeviceIndependentResources()
{
    HRESULT hr = S_OK;

    // Create a Direct2D factory.
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

    return hr;
}

/*Implement the DemoApp::CreateDeviceResources method. 
This method creates the window's device-dependent resources, a render target, and two brushes. 
Retrieve the size of the client area and create an ID2D1HwndRenderTarget of the same size that renders to the window's HWND. 
Store the render target in the m_pRenderTarget class member. 
Use the render target to create a gray ID2D1SolidColorBrush and a cornflower blue ID2D1SolidColorBrush.
Because this method will be called repeatedly, add an if statement to check whether the render target (m_pRenderTarget ) already exists.
The following code shows the complete CreateDeviceResources method.*/
HRESULT Temporal::CreateDeviceResources()
{
    HRESULT hr = S_OK;

    if (!m_pRenderTarget)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top
            );

        // Create a Direct2D render target.
        hr = m_pDirect2dFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &m_pRenderTarget
            );


        if (SUCCEEDED(hr))
        {
            // Create a gray brush.
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::LightSlateGray),
                &m_pLightSlateGrayBrush
                );
        }
        if (SUCCEEDED(hr))
        {
            // Create a blue brush.
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::CornflowerBlue),
                &m_pCornflowerBlueBrush
                );
        }
    }

    return hr;
}

/*Implement the DemoApp::DiscardDeviceResources method. 
In this method, release the render target and the two brushes you created in the DemoApp::CreateDeviceResources method.*/
void Temporal::DiscardDeviceResources()
{
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pLightSlateGrayBrush);
    SafeRelease(&m_pCornflowerBlueBrush);
}

/*Implement the DemoApp::WndProc method to handle window messages. 
For the WM_SIZE message, call the Temporal::OnResize method and pass it the new width and height. 
For the WM_PAINT and WM_DISPLAYCHANGE messages, call the Temporal::OnRender method to paint the window. 
You implement the OnRender and OnResize methods in the steps that follow.*/
LRESULT CALLBACK Temporal::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        Temporal *pTemporal = (Temporal *)pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            PtrToUlong(pTemporal)
            );

        result = 1;
    }
    else
    {
        Temporal *pTemporal = reinterpret_cast<Temporal *>(static_cast<LONG_PTR>(
            ::GetWindowLongPtrW(
                hwnd,
                GWLP_USERDATA
                )));

        bool wasHandled = false;

        if (pTemporal)
        {
			float NewX;
			float NewY;

            switch (message)
            {
            case WM_SIZE:
                {
                    UINT width = LOWORD(lParam);
                    UINT height = HIWORD(lParam);
					pTemporal->WindowWidth = width;
					pTemporal->WindowHeight = height;
                    pTemporal->OnResize(width, height);
					if( pTemporal->AllActors.size > 0)
					{
						((Mouse)pTemporal->AllActors[0])->MaxX = (float)width - 10;
						((Mouse)pTemporal->AllActors[0])->MaxY = (float)height - 10;
					}
                }
                result = 0;
                wasHandled = true;
                break;

            case WM_DISPLAYCHANGE:
                {
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                result = 0;
                wasHandled = true;
                break;

            case WM_PAINT:
                {
					pTemporal->bReadyToRender = true;
                    pTemporal->OnRender();
                    ValidateRect(hwnd, NULL);
                }
                result = 0;
                wasHandled = true;
                break;

			case WM_MOUSEMOVE:
				{
					if( pTemporal->AllActors[0] != NULL)
					{
						POINT MousePoint;
						GetCursorPos(&MousePoint);
						
						ScreenToClient(hwnd, &MousePoint);


						((Mouse*)pTemporal->AllActors[0])->LocX = MousePoint.x;
						((Mouse*)pTemporal->AllActors[0])->LocY = MousePoint.y;
						if(((Mouse*)pTemporal->AllActors[0])->FitBounds())
						{
							ShowCursor(true);
						}
						else
						{
							ShowCursor(false);
						}
					}
				}
				result = 0;
                wasHandled = true;
				break;

			case WM_MOUSELEAVE:
				ShowCursor(true);
				break;

			//Register Clicks
			case WM_LBUTTONDOWN:
				if(pTemporal->CurrentState == STATE_GAME)
				{
					NewX = pTemporal->AllActors[0]->LocX;
					NewY = pTemporal->AllActors[0]->LocY;
					pTemporal->Spawn(CLASS_TANK, NewX, NewY, SCALE_TANK, TEAM1);
				}
				
				if(pTemporal->CurrentState == STATE_MAINMENU)
				{
					pTemporal->GoToState(STATE_CONNECTING);
					pTemporal->ConnSocket->ClientConnect("127.0.0.1", 12345);
					char* ReturnMessage = pTemporal->SendClientInfo();
					pTemporal->ProcessMessage(ReturnMessage);
					free(ReturnMessage);
				}
					break;

			case WM_RBUTTONDOWN:
				if(pTemporal->CurrentState == STATE_GAME)
				{
					NewX = pTemporal->AllActors[0]->LocX;
					NewY = pTemporal->AllActors[0]->LocY;
					pTemporal->Spawn(CLASS_TANK, NewX, NewY, SCALE_TANK, TEAM2);
				}
				if(pTemporal->CurrentState == STATE_MAINMENU)
				{
					pTemporal->GoToState(STATE_SERVERWAIT);
					pTemporal->ConnSocket->ServerHost(12345);
				}
					break;

			case WM_MBUTTONDOWN:
					break;

            case WM_DESTROY:
                {
                    PostQuitMessage(0);
                }
                result = 1;
                wasHandled = true;
                break;
            }
        }

        if (!wasHandled)
        {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}

int Temporal::Spawn(SpawnClass pClass, float pX, float pY, ActorScale pScale, Teams pTeam)
{
	switch (pClass)
	{
	case CLASS_ACTOR:
		AllActors.push_back(new Actor());
		break;
	case CLASS_MOUSE:
		AllActors.push_back(new Mouse());
		break;
	case CLASS_TANK:
		AllActors[i] = new Tank();
		break;
	case CLASS_GUITEXT:
		AllActors[i] = new GUIText();
		break;
	default:
		break;
	}
			
	AllActors[i]->LocX = pX;
	AllActors[i]->LocY = pY;
	AllActors[i]->Scale = pScale;
	AllActors[i]->ClassName = pClass;

	switch (pTeam)
	{
	case TEAM1:
		AllActors[i]->BorderColor = COLOR_ALLY;
		AllActors[i]->FillColor = COLOR_ALLY;
		break;

	case TEAM2:
		AllActors[i]->BorderColor = COLOR_ENEMY;
		AllActors[i]->FillColor = COLOR_ENEMY;
		break;

	case NEUTRAL:
		AllActors[i]->BorderColor = COLOR_NEUTRAL;
		AllActors[i]->FillColor = COLOR_NEUTRAL;
		break;

	default:
		AllActors[i]->BorderColor = COLOR_NEUTRAL;
		AllActors[i]->FillColor = COLOR_NEUTRAL;
		break;
	}

	AllActors[i]->CurrentTeam = pTeam;

	return i;
}


double Temporal::DeltaTime()
{
	//Get the currentClockTime
	clock_t CurrentTime = clock();
	
	//Get the DeltaTime since last Call
	double returnDTime = (CurrentTime - LastTime)/CLOCKS_PER_SEC;

	LastTime = CurrentTime;

	//return the deltaTime
	return returnDTime;
}

//Deletes the actor at the given index
void Temporal::DeleteActor(int index)
{
	free(AllActors[index]);
	AllActors[index] = NULL;
}

void Temporal::InitMouse()
{
	AllActors[0] = new Mouse();
}


void Temporal::initState(StateEnum pState)
{
	//Clear all Actors
	for(int i=0; i<100; i++)
	{
		DeleteActor(i);
	}
	
	InitMouse();

	int SpawnIndex = -1;

	switch (pState)
	{
	case STATE_MAINMENU:
		SpawnIndex = Spawn(CLASS_GUITEXT, 200.0f,300.0f, SCALE_TEXT, NEUTRAL);
		if(SpawnIndex != -1)
		{
			char NewTitle[100] = "Temporal!\n LeftClick to join server, RightClick to Host\0";

			((GUIText*)AllActors[SpawnIndex])->SetMessage(NewTitle);
		}
		break;
	case STATE_GAME:
		break;
	case STATE_WIN:
		break;
	case STATE_LOSE:
		break;
	case STATE_SERVERWAIT:
		SpawnIndex = Spawn(CLASS_GUITEXT, 200.0f,300.0f, SCALE_TEXT, NEUTRAL);
		if(SpawnIndex != -1)
		{
			char NewTitle[100] = "Waiting for Clients.....\0";

			((GUIText*)AllActors[SpawnIndex])->SetMessage(NewTitle);
		}
		break;
		break;
	case STATE_SERVERCONNECTED:
		SpawnIndex = Spawn(CLASS_GUITEXT, 200.0f,300.0f, SCALE_TEXT, NEUTRAL);
		if(SpawnIndex != -1)
		{
			char NewTitle[100] = "Hosting Server!\0";

			((GUIText*)AllActors[SpawnIndex])->SetMessage(NewTitle);
		}
		break;
	case STATE_CONNECTING:
		SpawnIndex = Spawn(CLASS_GUITEXT, 200.0f,300.0f, SCALE_TEXT, NEUTRAL);
		if(SpawnIndex != -1)
		{
			char NewTitle[100] = "Connecting!.........\0";

			((GUIText*)AllActors[SpawnIndex])->SetMessage(NewTitle);
		}
		break;
	default:
		break;
	}
}

void Temporal::GoToState(StateEnum pState)
{
	CurrentState = pState;

	initState(pState);
}

void Temporal::ProcessMessage(char* pServerMessage)
{
	char* Token;
	Token = strtok(pServerMessage, ":");

	int CommandToken = (int) *Token;

	switch (CommandToken)
	{
	case CONFIRMADD:
		GoToState(STATE_GAME);
		break;
	case UPDATEALLACTORS:
		break;
	default:
		break;
	}
}

ClientCommand Temporal::ServerProcessMessage(char* pClientMessage)
{
	char* Token;
	Token = strtok(pClientMessage, ":");

	int CommandToken = atoi(Token);

	switch (CommandToken)
	{
	case INITCLIENT:
		Token = strtok(NULL, ":");
		RegisterNewClient(Token);
		return CONFIRMADD;
		break;

	case ADDUNIT:
		SpawnClass newClass;
		float pX, pY;
		ActorScale newScale;
		Teams newTeam;

		Token = strtok(NULL, ",");
		newClass = (SpawnClass)*Token;

		Token = strtok(NULL, ",");
		pX = (float)*Token;

		Token = strtok(NULL, ",");
		pY = (float)*Token;

		Token = strtok(NULL, ",");
		newScale = (ActorScale)*Token;

		Token = strtok(NULL, ",");
		newTeam = (Teams)*Token;

		Spawn(newClass, pX, pY, newScale, newTeam);
		return UPDATEALLACTORS;
		break;

	default:
		break;
	}

	free(Token);

}

char* Temporal::SendClientInfo()
{
	//If we have a Valid connection
	if(ConnSocket->CurrentSocket != -1)
	{
		char* pMessage = (char*)malloc(1024);
		memset(pMessage, '\0', 1024);
		sprintf(pMessage,"%d:%s",INITCLIENT, "ClientName");

		return ConnSocket->ClientSend(pMessage);
	}
}

void Temporal::RegisterNewClient(char* ClientName)
{
	for(int i = 0; i < 16; i++)
	{
		if(ConnSocket->AllConnClients[i] == NULL)
		{
			strcpy(ConnSocket->AllConnClients[i], ClientName);
		}
	}
}


