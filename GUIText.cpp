#include "GUIText.h"


GUIText::GUIText(void)
{
	memset(Message, '\0', 1024);
}


GUIText::~GUIText(void)
{
}



void GUIText::Draw(HWND m_hwnd, ID2D1HwndRenderTarget* pCanvas, ID2D1Factory* m_pDirect2dFactory)
{
	SetFillColor(FillColor, pCanvas);
	SetBorderColor(BorderColor, pCanvas);

	PaintText(m_hwnd, Message, LocX, LocY);
}

void GUIText::SetMessage(char* pMessage)
{
	strcpy(Message, pMessage);
}
