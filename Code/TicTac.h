#pragma once

#include "Matrix.h"
#include <map>

struct Position {
    matrix<int> board;
    Position( ) : board( 3, 3 ) { }
    Position( const matrix<int> &theBoard ) : board( theBoard ) { }
    bool operator<( const Position &rhs ) const;
};

typedef map<Position,int,less<Position>> MapType;
typedef MapType::const_iterator MapItr;

class TicTacToe {
  public:
	matrix<int> board;
	enum Side { HUMAN, COMPUTER, EMPTY };
	enum PositionVal { HUMAN_WIN, DRAW, UNCLEAR, COMPUTER_WIN };
	TicTacToe( ) : board( 3, 3 ) { clearBoard( ); }
	int chooseMove( Side d, int & bestRow, int & bestColumn, int alpha = HUMAN_WIN, int beta = COMPUTER_WIN, int depth = 0 );
	bool playMove( Side s, int row, int column );
	void clearBoard( );
	bool boardIsFull( ) const;
	bool isAWin( Side s ) const;
  private:
	MapType transpositions;
	void place( int row, int column, int piece = EMPTY ) { board[ row ][ column ] = piece; }
	bool squareIsEmpty( int row, int column ) const { return board[ row ][ column ] == EMPTY; }
	int positionValue( ) const;
};