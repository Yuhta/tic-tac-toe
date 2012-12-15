#include "Canvas.h"
#include "Except.h"
#include <windowsx.h>

CANVAS canvas;

static BOOL BlockAnalyze( INT xPos, INT yPos, INT & r, INT & c ) {
	if( xPos < 135 || xPos > 463 || yPos < 36 || yPos > 364 ) return FALSE;
	else {
		c = ( xPos - 135 ) / 110, r = ( yPos - 36 ) / 110;
		return TRUE;
	}
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	INT xPos, yPos;
	switch( msg ) {
		case WM_DESTROY:
			PostQuitMessage( 0 );
			return 0;
		case WM_MOUSEMOVE:
			xPos = GET_X_LPARAM( lParam ), yPos = GET_Y_LPARAM( lParam );
			canvas.button[ 0 ] = ( xPos > 20 && xPos < 110 && yPos > 31 && yPos < 107 );
			canvas.button[ 1 ] = ( xPos > 487 && xPos < 574 && yPos > 297 && yPos < 367 );
			return 0;
		case WM_LBUTTONDOWN:
			if( canvas.button[ 0 ] ) {
				canvas.compGoesFirst = TRUE;
				canvas.state = TicTacToe::UNCLEAR;
				canvas.tic.clearBoard( );
				canvas.tic.playMove( TicTacToe::COMPUTER, canvas.gameNum % 3, canvas.gameNum / 3 % 3);
				++canvas.gameNum;
			} else if( canvas.button[ 1 ] ) {
				canvas.compGoesFirst = FALSE;
				canvas.state = TicTacToe::UNCLEAR;
				canvas.tic.clearBoard( );
			} else if( canvas.state == TicTacToe::UNCLEAR ) {
				xPos = GET_X_LPARAM( lParam ), yPos = GET_Y_LPARAM( lParam );
				INT r, c;
				if( BlockAnalyze( xPos, yPos, r, c ) && canvas.tic.playMove( TicTacToe::HUMAN, r, c ) )
					if( canvas.tic.isAWin( TicTacToe::HUMAN ) ) canvas.state = TicTacToe::HUMAN_WIN;
					else if ( canvas.tic.boardIsFull( ) ) canvas.state = TicTacToe::DRAW;
					else {
						canvas.tic.chooseMove( TicTacToe::COMPUTER, r, c );
						canvas.tic.playMove( TicTacToe::COMPUTER, r, c );
						if( canvas.tic.isAWin( TicTacToe::COMPUTER ) ) canvas.state = TicTacToe::COMPUTER_WIN;
						else if ( canvas.tic.boardIsFull( ) ) canvas.state = TicTacToe::DRAW;
					}
			}
			return 0;
	}
	return DefWindowProc( hWnd, msg, wParam, lParam );
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, PSTR, INT ) {
	try { canvas.Initialize( hInstance ); }
	catch( EXCEPTION e ) {
		MessageBox( 0, e.GetMsg( ), 0, 0 );
		return 1;
	}
	canvas.EnterMsgLoop( );
	return 0;
}