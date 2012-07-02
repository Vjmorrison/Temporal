#include "Actor.h"

Actor::Actor()
{
	DrawShape = SHAPE_SQUARE;
	FillColor = COLOR_ALLY;
	BorderColor = COLOR_ALLY;
	
	LocX = 0.0f;
	LocY = 0.0f;
	Scale = 20.0f;

	for(int i = 0; i<50; i++)
	{
		AttachedActors[i] = NULL;
	}
}

//Define how this Actor needs to be drawn.
void Actor::Draw(HWND m_hwnd, ID2D1HwndRenderTarget* pCanvas, ID2D1Factory* m_pDirect2dFactory)
{
	D2D1_RECT_F rectangle2;

	SetFillColor(FillColor, pCanvas);
	SetBorderColor(BorderColor, pCanvas);

	switch (DrawShape)
	{
	case SHAPE_SQUARE:
		rectangle2 = D2D1::RectF(
            LocX - Scale/2,
            LocY - Scale/2,
            LocX + Scale/2,
            LocY + Scale/2
            );

		// Draw a filled rectangle.
		pCanvas->FillRectangle(&rectangle2, FillBrush);
		break;

	case SHAPE_CIRCLE:
		ID2D1EllipseGeometry *m_pEllipseGeometry;

        m_pDirect2dFactory->CreateEllipseGeometry(
        D2D1::Ellipse(D2D1::Point2F(LocX, LocY), Scale, Scale),
        &m_pEllipseGeometry
        );

		pCanvas->FillGeometry(m_pEllipseGeometry, FillBrush);
		break;

	case SHAPE_LINE:
		pCanvas->DrawLine(
                D2D1::Point2F(LocX, LocY),
				D2D1::Point2F(LocX+Scale, LocY+Scale),
				BorderBrush,
                0.5f
                );
		break;

	default:
		rectangle2 = D2D1::RectF(
            LocX - Scale/2,
            LocY - Scale/2,
            LocX + Scale/2,
            LocY + Scale/2
            );

		// Draw a filled rectangle.
		pCanvas->FillRectangle(&rectangle2, FillBrush);
		break;
	}

	for(int i = 0; i<50; i++)
	{
		if(AttachedActors[i] != NULL)
		{
			AttachedActors[i]->Draw(m_hwnd, pCanvas, m_pDirect2dFactory);
		}
	}
}

void Actor::SetBorderColor(ActorColor pColor, ID2D1HwndRenderTarget* pCanvas)
{
	pCanvas->CreateSolidColorBrush( D2D1::ColorF(pColor), &BorderBrush);
}

void Actor::SetFillColor(ActorColor pColor, ID2D1HwndRenderTarget* pCanvas)
{
	pCanvas->CreateSolidColorBrush( D2D1::ColorF(pColor), &FillBrush);
}

void Actor::Tick(double* DeltaTime)
{
	//DO your Logic!
}

void Actor::PaintText(HWND m_hwnd, char *Message, float X, float Y)
{
	RECT rect;
	HDC hdc = GetDC(m_hwnd);  
	TCHAR szBuffer[100] = {0};   

	int iLength = 0;

	iLength = sprintf(szBuffer, Message);

	SetRect(&rect, X, Y, X + 800, Y + 34);

	DrawText(hdc, szBuffer, iLength, &rect, 32);
}