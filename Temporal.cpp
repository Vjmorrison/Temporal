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

	for(int i = 0; i < 100; i++)
	{
		AllActors[i] = NULL;
	}

	AllActors[0] = new Mouse();

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
                    pTemporal->OnResize(width, height);
					if( pTemporal->AllActors[0] != NULL)
					{
						((Mouse*)pTemporal->AllActors[0])->MaxX = (float)width - 10;
						((Mouse*)pTemporal->AllActors[0])->MaxY = (float)height - 10;
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
					ShowCursor(false);
					if( pTemporal->AllActors[0] != NULL)
					{
						POINT MousePoint;
						GetCursorPos(&MousePoint);
						
						ScreenToClient(hwnd, &MousePoint);


						((Mouse*)pTemporal->AllActors[0])->LocX = MousePoint.x;
						((Mouse*)pTemporal->AllActors[0])->LocY = MousePoint.y;
						((Mouse*)pTemporal->AllActors[0])->FitBounds();
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
				NewX = pTemporal->AllActors[0]->LocX;
				NewY = pTemporal->AllActors[0]->LocY;
				pTemporal->Spawn(NewX, NewY, SCALE_TANK, TEAM1);
					break;

			case WM_RBUTTONDOWN:
				NewX = pTemporal->AllActors[0]->LocX;
				NewY = pTemporal->AllActors[0]->LocY;
				pTemporal->Spawn(NewX, NewY, SCALE_TANK, TEAM2);
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

bool Temporal::Spawn(float pX, float pY, ActorScale pScale, Teams pTeam)
{
	for(int i = 0; i<100; i++)
	{
		if(AllActors[i] == NULL)
		{
			AllActors[i] = new Actor();
			AllActors[i]->LocX = pX;
			AllActors[i]->LocY = pY;
			AllActors[i]->Scale = pScale;

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
			return true;
		}
	}
	return false;
}

