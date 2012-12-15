#include "Canvas.h"
#include "Except.h"

#define WS_CUSTOMWINDOWSTYLE ( WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX )
#define RELEASE( lp ) if( lp != 0 ) lp->Release( )

extern LRESULT CALLBACK WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

struct CANVAS::BACKVERTEX {
	FLOAT x, y, z, rhw, tu, tv;
	static CONST INT FVF;
};
CONST INT CANVAS::BACKVERTEX::FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;

struct CANVAS::POINTVERTEX {
	FLOAT x, y, z, rhw;
	static CONST INT FVF;
};
CONST INT CANVAS::POINTVERTEX::FVF = D3DFVF_XYZRHW;

static DWORD FtoDw( FLOAT f ) { return *( ( DWORD * ) &f ); }

CANVAS::CANVAS( ): g_pd3dDevice( 0 ), g_pBackVB( 0 ), g_pPointVB( 0 ), g_pFont( 0 ), compGoesFirst( FALSE ), gameNum( 0 ), state( TicTacToe::UNCLEAR ) {
	ZeroMemory( button, sizeof( button ) );
	ZeroMemory( g_pPics, sizeof( g_pPics ) );
}

CANVAS::~CANVAS( ) {
	RELEASE( g_pFont );
	for( UINT i = 0; i < 6; ++i ) RELEASE( g_pPics[ i ] );
	RELEASE( g_pPointVB );
	RELEASE( g_pBackVB );
	RELEASE( g_pd3dDevice );
}

VOID CANVAS::Initialize( HINSTANCE hInstance ) {
	InitD3D( hInstance, 600, 400, true, D3DDEVTYPE_HAL );
	if( FAILED( InitVB( ) ) ) throw EXCEPTION( L"Failed to initialize vertex buffer!" );
	if( FAILED( InitPics( ) ) ) throw EXCEPTION( L"Cannot find picture files!" );
	InitFont( );
}

VOID CANVAS::Display( FLOAT timeDelta ) {
	if( SUCCEEDED( g_pd3dDevice->BeginScene( ) ) ) {
		g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		g_pd3dDevice->SetStreamSource( 0, g_pBackVB, 0, sizeof( BACKVERTEX ) );
		g_pd3dDevice->SetFVF( BACKVERTEX::FVF );
		g_pd3dDevice->SetTexture( 0, g_pPics[ 0 ] );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
		g_pd3dDevice->SetStreamSource( 0, g_pPointVB, 0, sizeof( POINTVERTEX ) );
		g_pd3dDevice->SetFVF( POINTVERTEX::FVF );
		g_pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
		g_pd3dDevice->SetRenderState( D3DRS_POINTSIZE, FtoDw( 56 ) );
		if( button[ 0 ] ) g_pd3dDevice->SetTexture( 0, g_pPics[ 3 ] );
		else g_pd3dDevice->SetTexture( 0, g_pPics[ 1 ] );
		g_pd3dDevice->DrawPrimitive( D3DPT_POINTLIST, 0, 1 );
		if( button[ 1 ] ) g_pd3dDevice->SetTexture( 0, g_pPics[ 2 ] );
		else g_pd3dDevice->SetTexture( 0, g_pPics[ 1 ] );
		g_pd3dDevice->DrawPrimitive( D3DPT_POINTLIST, 1, 1 );
		g_pd3dDevice->SetRenderState( D3DRS_POINTSIZE, FtoDw( 104 ) );
		UINT human, comp;
		if( compGoesFirst ) comp = 4, human = 5;
		else human = 4, comp = 5;
		g_pd3dDevice->SetTexture( 0, g_pPics[ human ] );
		g_pd3dDevice->DrawPrimitive( D3DPT_POINTLIST, 2, 1 );
		g_pd3dDevice->SetTexture( 0, g_pPics[ comp ] );
		g_pd3dDevice->DrawPrimitive( D3DPT_POINTLIST, 3, 1 );
		for( INT r = 0; r < 3; ++r )
			for( INT c = 0; c < 3; ++c ) {
				if ( tic.board[ r ][ c ] == TicTacToe::EMPTY ) continue;
				if ( tic.board[ r ][ c ] == TicTacToe::HUMAN ) g_pd3dDevice->SetTexture( 0, g_pPics[ human ] );
				else g_pd3dDevice->SetTexture( 0, g_pPics[ comp ] );
				g_pd3dDevice->DrawPrimitive( D3DPT_POINTLIST, r * 3 + c + 4, 1 );
			}
		g_pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
		if( state != TicTacToe::UNCLEAR ) {
			LPWSTR text;
			switch( state ) {
				case TicTacToe::HUMAN_WIN:
					text = L"Human wins!!";
					break;
				case TicTacToe::COMPUTER_WIN:
					text = L"Computer wins!!";
					break;
				case TicTacToe::DRAW:
					text = L"Draw!!";
			}
			RECT rc = { 300, 368, 300 };
			g_pFont->DrawText( 0, text, -1, &rc, DT_NOCLIP | DT_CENTER, 0xFFFF0000 );
		}
		g_pd3dDevice->EndScene( );
	}
	g_pd3dDevice->Present( 0, 0, 0, 0 );
}

VOID CANVAS::InitD3D( HINSTANCE hInstance, INT width, INT height, bool windowed, D3DDEVTYPE deviceType ) {
	WNDCLASSEX wc = { sizeof( WNDCLASSEX ), CS_CLASSDC, WndProc, 0, 0, GetModuleHandle( 0 ), 0, 0, 0, 0, L"Window", 0 };
	if( !RegisterClassEx( &wc ) ) throw EXCEPTION( L"RegisterClass( ) - FAILED" );
	HWND hwnd = 0;
	hwnd = CreateWindow( L"Window", L"Tic-Tac-Toe", WS_CUSTOMWINDOWSTYLE, 0, 0, width, height, 0, 0, hInstance, 0);
	if( !hwnd ) throw EXCEPTION( L"CreateWindow( ) - FAILED" );
	RECT clientRect, wndRect;
	GetClientRect( hwnd, &clientRect );
	GetWindowRect( hwnd, &wndRect );
	DWORD wndWidth = wndRect.right - wndRect.left - clientRect.right + width;
	DWORD wndHeight = wndRect.bottom - wndRect.top - clientRect.bottom + height;
	MoveWindow( hwnd, wndRect.left, wndRect.top, wndWidth, wndHeight, TRUE );
	ShowWindow( hwnd, SW_SHOW );
	UpdateWindow( hwnd );
	HRESULT hr = 0;
	IDirect3D9* d3d9 = 0;
    d3d9 = Direct3DCreate9( D3D_SDK_VERSION );
    if( !d3d9 ) throw EXCEPTION( L"Direct3DCreate9( ) - FAILED" );
	D3DCAPS9 caps;
	d3d9->GetDeviceCaps( D3DADAPTER_DEFAULT, deviceType, &caps );
	INT vp = 0;
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(	&d3dpp, sizeof( d3dpp ) );
	d3dpp.Windowed = windowed;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	hr = d3d9->CreateDevice( D3DADAPTER_DEFAULT, deviceType, hwnd, vp, &d3dpp, &g_pd3dDevice );
	d3d9->Release( );
	if( FAILED( hr ) ) throw EXCEPTION( L"CreateDevice( ) - FAILED" );
}

HRESULT CANVAS::InitVB( ) {
	BACKVERTEX back[ ] = {
		{   0,   0, 0.5, 1, 0, 0 },
		{ 600,   0, 0.5, 1, 1, 0 },
		{   0, 400, 0.5, 1, 0, 1 },
		{ 600, 400, 0.5, 1, 1, 1 },
	};
	if( FAILED( g_pd3dDevice->CreateVertexBuffer( sizeof( back ) , D3DUSAGE_WRITEONLY, BACKVERTEX::FVF, D3DPOOL_MANAGED, &g_pBackVB, 0 ) ) ) return E_FAIL;
	BACKVERTEX * pBack;
	if( FAILED( g_pBackVB->Lock( 0, 0, ( VOID ** )&pBack, 0 ) ) ) return E_FAIL;
	memcpy( pBack, back, sizeof( back ) );
	g_pBackVB->Unlock();
	POINTVERTEX points[ ] = {
		{  66,  64, 0.5, 1 },
		{ 532, 344, 0.5, 1 },
		{  68, 274, 0.5, 1 },
		{ 530, 125, 0.5, 1 },
	};
	if( FAILED( g_pd3dDevice->CreateVertexBuffer( 13 * sizeof( POINTVERTEX ) , D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS, POINTVERTEX::FVF, D3DPOOL_MANAGED, &g_pPointVB, 0 ) ) ) return E_FAIL;
	POINTVERTEX * pPoints;
	if( FAILED( g_pPointVB->Lock( 0, 0, ( VOID ** )&pPoints, 0 ) ) ) return E_FAIL;
	memcpy( pPoints, points, sizeof( points ) );
	pPoints += 4;
	for( UINT i = 0; i < 3; ++i )
		for( UINT j = 0; j < 3; ++j )
			pPoints->x = 189.667f + j * 109.333f,
			pPoints->y = 90.667f + i * 109.333f,
			pPoints->z = 0.5, pPoints->rhw = 1, ++pPoints;
	g_pPointVB->Unlock( );
	return S_OK;
}

HRESULT CANVAS::InitPics( ) {
	if( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, L".\\pics\\background.dds", &g_pPics[ 0 ] ) ) ) return E_FAIL;
	if( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, L".\\pics\\reset.dds", &g_pPics[ 1 ] ) ) ) return E_FAIL;
	if( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, L".\\pics\\reset1.dds", &g_pPics[ 2 ] ) ) ) return E_FAIL;
	if( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, L".\\pics\\reset2.dds", &g_pPics[ 3 ] ) ) ) return E_FAIL;
	if( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, L".\\pics\\inyou.dds", &g_pPics[ 4 ] ) ) ) return E_FAIL;
	if( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, L".\\pics\\star.dds", &g_pPics[ 5 ] ) ) ) return E_FAIL;
	return S_OK;
}

VOID CANVAS::InitFont( ) {
	D3DXFONT_DESC lf;
	ZeroMemory( &lf, sizeof( lf ) );
	lf.CharSet = DEFAULT_CHARSET;
	lf.Quality = PROOF_QUALITY;
	lf.Weight = 500;
	lf.Height = 30;
	wcscpy_s( lf.FaceName, 7, L"Impact" );
	if( FAILED( D3DXCreateFontIndirect( g_pd3dDevice, &lf, &g_pFont ) ) ) throw EXCEPTION( L"Cannot create font!" );
}

VOID CANVAS::EnterMsgLoop( ) {
	MSG msg;
	ZeroMemory( &msg, sizeof( MSG ) );
	static float lastTime = ( float )timeGetTime( ); 
	while( msg.message != WM_QUIT )
		if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) ) {
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		} else {	
			float currTime  = ( float )timeGetTime( );
			float timeDelta = ( currTime - lastTime ) * 0.001f;
			Display( timeDelta );
			lastTime = currTime;
        }
}
