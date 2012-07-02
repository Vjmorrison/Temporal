#include "Temporal.h"


/*Implement the Temporal::WndProc method to handle window messages. 
For the WM_SIZE message, call the DemoApp::OnResize method and pass it the new width and height. 
For the WM_PAINT and WM_DISPLAYCHANGE messages, call the Temporal::OnRender method to paint the window. 
You implement the OnRender and OnResize methods in the steps that follow.*/
HRESULT Temporal::OnRender()
{
    HRESULT hr = S_OK;

    hr = CreateDeviceResources();

	if (SUCCEEDED(hr))
    {
		//Get the DeltaTime Since the last Loop.
		double DT = DeltaTime();

		m_pRenderTarget->BeginDraw();

        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

		if(bReadyToRender)
		{
			//State Switch
			switch (CurrentState)
			{
			case STATE_MAINMENU:
				DrawMenu();
				break;
			case STATE_GAME:
				DrawGame(&DT);
				break;
			case STATE_WIN:
				DrawWin();
				break;
			case STATE_LOSE:
				DrawLose();
				break;
			case STATE_SERVERWAIT:
				DrawWaiting("Waiting For Connections");
				break;
			case STATE_SERVERCONNECTED:
				DrawServerScreen();
				break;
			case STATE_CONNECTING:
				DrawWaiting("Connecting");
				break;
			default:
				DrawWaiting("Default");
				break;
			}

			
		}

		hr = m_pRenderTarget->EndDraw();
		/*
		RECT rect;
		HDC hdc = GetDC(m_hwnd);  
		TCHAR szBuffer[100] = {0};   

		int iLength = 0;
		float LocX = AllActors[0]->LocX;
		float LocY = AllActors[0]->LocY;

		iLength = sprintf(szBuffer, "LocX: %g    LocY: %g",LocX, LocY);

		SetRect(&rect, 0, 0, 0 + 800, 0 + 34);

		DrawText(hdc, szBuffer, iLength, &rect, 32);
		*/
		
	}

	if (hr == D2DERR_RECREATE_TARGET)
    {
        hr = S_OK;
        DiscardDeviceResources();
    }

	return hr;

}

/*Implement the DemoApp::OnResize method so that it resizes the render target to the new size of the window.*/
void Temporal::OnResize(UINT width, UINT height)
{
    if (m_pRenderTarget)
    {
        // Note: This method can fail, but it's okay to ignore the
        // error here, because the error will be returned again
        // the next time EndDraw is called.
        m_pRenderTarget->Resize(D2D1::SizeU(width, height));
    }
}

//Drawing the main menu
bool Temporal::DrawMenu()
{
	for( int i = 0; i < 100; i++)
	{
		if(AllActors[i] != NULL)
		{
			if(i==0)
			{
				((Mouse *)AllActors[i])->Draw(m_hwnd, m_pRenderTarget, m_pDirect2dFactory);
			}
			else
			{
				m_pRenderTarget->EndDraw();
				((GUIText*)AllActors[i])->Draw(m_hwnd, m_pRenderTarget, m_pDirect2dFactory);
			}
		}
	}
	return true;
}

//Drawing the main game
bool Temporal::DrawGame(double* DT)
{
	//Draw All objects in the ALLActors Cue
	for( int i = 0; i < 100; i++)
	{
		if(AllActors[i] != NULL)
		{
			if((Mouse *) AllActors[i] != NULL)
			{
				((Mouse *)AllActors[i])->Draw(m_hwnd, m_pRenderTarget, m_pDirect2dFactory);
			}
			else
			{
				AllActors[i]->Draw(m_hwnd, m_pRenderTarget, m_pDirect2dFactory);
				AllActors[i]->Tick(DT);
			}
		}
	}

	return true;
}

bool Temporal::DrawWin()
{
	return true;
}

bool Temporal::DrawLose()
{
	return true;
}

bool Temporal::DrawWaiting(char* Message)
{

	for( int i = 0; i < 100; i++)
	{
		if(AllActors[i] != NULL)
		{
			if(i==0)
			{
				((Mouse *)AllActors[i])->Draw(m_hwnd, m_pRenderTarget, m_pDirect2dFactory);
			}
			else
			{
				m_pRenderTarget->EndDraw();
				((GUIText*)AllActors[i])->SetMessage(Message);
				((GUIText*)AllActors[i])->Draw(m_hwnd, m_pRenderTarget, m_pDirect2dFactory);
			}
		}
	}
	return true;
}

bool Temporal::DrawServerScreen()
{
	for( int i = 0; i < 100; i++)
	{
		if(AllActors[i] != NULL)
		{
			if(i==0)
			{
				((Mouse *)AllActors[i])->Draw(m_hwnd, m_pRenderTarget, m_pDirect2dFactory);
			}
			else
			{
				m_pRenderTarget->EndDraw();
				((GUIText*)AllActors[i])->Draw(m_hwnd, m_pRenderTarget, m_pDirect2dFactory);
			}
		}
	}
	return true;
}

