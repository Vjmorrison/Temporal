#include "Actor.h"

Actor::Actor()
{
	DrawShape = SHAPE_SQUARE;
	FillColor = COLOR_ALLY;
	BorderColor = COLOR_ALLY;
	
	LocX = 0.0f;
	LocY = 0.0f;
	Scale = 20.0f;
}

//Define how this Actor needs to be drawn.
void Actor::Draw(ID2D1HwndRenderTarget* pCanvas, ID2D1Factory* m_pDirect2dFactory)
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
}

void Actor::SetBorderColor(ActorColor pColor, ID2D1HwndRenderTarget* pCanvas)
{
	pCanvas->CreateSolidColorBrush( D2D1::ColorF(pColor), &BorderBrush);
}

void Actor::SetFillColor(ActorColor pColor, ID2D1HwndRenderTarget* pCanvas)
{
	pCanvas->CreateSolidColorBrush( D2D1::ColorF(pColor), &FillBrush);
}