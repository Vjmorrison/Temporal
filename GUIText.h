#ifndef _GUITEXT
#define _GUITEXT

#include "actor.h"
class GUIText :
	public Actor
{
public:

	char Message[1024];

	GUIText(void);
	~GUIText(void);

	void SetMessage(char* pMessage);

	void Draw(HWND m_hwnd, ID2D1HwndRenderTarget* pCanvas, ID2D1Factory* m_pDirect2dFactory);
};

#endif

