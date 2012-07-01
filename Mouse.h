#ifndef _MOUSEDEF
#define _MOUSEDEF

#include "Actor.h"



class Mouse : public Actor
{
public:
	Mouse(void);
	~Mouse(void);
	float MinX;
	float MaxX;
	float MinY;
	float MaxY;
	bool FitBounds(void);


};

#endif

