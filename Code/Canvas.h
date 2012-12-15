#pragma once

#include "TicTac.h"
#include <d3dx9.h>

class CANVAS {
  public:
	TicTacToe tic;
	BOOL compGoesFirst;
	TicTacToe::PositionVal state;
	UINT gameNum;
	BOOL button[ 2 ];
	CANVAS( );
	~CANVAS( );
	VOID Initialize( HINSTANCE hInstance );
	VOID EnterMsgLoop( );
  private:
	LPDIRECT3DDEVICE9 g_pd3dDevice;
	struct BACKVERTEX;
	struct POINTVERTEX;
	LPDIRECT3DVERTEXBUFFER9 g_pBackVB, g_pPointVB;
	LPDIRECT3DTEXTURE9 g_pPics[ 6 ];
	LPD3DXFONT g_pFont;
	VOID InitD3D( HINSTANCE hInstance, INT width, INT height, bool windowed, D3DDEVTYPE deviceType );
	HRESULT InitVB( );
	HRESULT InitPics( );
	VOID InitFont( );
	VOID Display( FLOAT timeDelta );
};