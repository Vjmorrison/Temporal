#include "Mouse.h"


Mouse::Mouse(void)
{
	MinX = 10.0f;
	MaxX = 500.0f;
	MinY = 10.0f;
	MaxY = 400.0f;

	Scale = 5.0f;

	DrawShape = SHAPE_CIRCLE;
	FillColor = COLOR_MOUSE;
	BorderColor = COLOR_MOUSE;

}


Mouse::~Mouse(void)
{
}


bool Mouse::FitBounds(void)
{
	bool bNeedFit = false;
	
	if(LocX > MaxX)
	{
		LocX = MaxX;
		bNeedFit = true;
	}
	if(LocX < MinX)
	{
		LocX = MinX;
		bNeedFit = true;
	}

	if(LocY > MaxY)
	{
		LocY = MaxY;
		bNeedFit = true;
	}
	if(LocY < MinY)
	{
		LocY = MinY;
		bNeedFit = true;
	}
	
	return bNeedFit;
}
