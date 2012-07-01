#ifndef _ACTORDEF
#define _ACTORDEF


#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>


enum ActorShape
{
	SHAPE_SQUARE,
	SHAPE_CIRCLE,
	SHAPE_LINE
};

enum ActorColor
{
	COLOR_ALLY = D2D1::ColorF::CornflowerBlue,
	COLOR_ENEMY = D2D1::ColorF::Red,
	COLOR_MOUSE = D2D1::ColorF::Green,
	COLOR_NEUTRAL = D2D1::ColorF::Gray
};

enum ActorScale
{
	SCALE_TANK = 20,
	SCALE_MOUSE = 10
};

enum Teams
{
	TEAM1,
	TEAM2,
	NEUTRAL
};

class Actor
{

public:
    Actor();

	ActorShape DrawShape;
	ActorColor FillColor;
	ActorColor BorderColor;
	ID2D1SolidColorBrush* BorderBrush;
	ID2D1SolidColorBrush* FillBrush;
	ID2D1Factory* Canvas2DFactory;

	float LocX;
	float LocY;
	float Scale;

	void Draw(ID2D1HwndRenderTarget* pCanvas, ID2D1Factory* m_pDirect2dFactory);

	void SetBorderColor(ActorColor pColor, ID2D1HwndRenderTarget* pCanvas);
	void SetFillColor(ActorColor pColor, ID2D1HwndRenderTarget* pCanvas);
};

#endif