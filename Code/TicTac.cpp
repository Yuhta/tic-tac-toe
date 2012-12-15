#include "TicTac.h"

bool Position::operator<( const Position &rhs ) const {
    for( int i = 0; i < 3; i++ )
        for( int j = 0; j < 3; j++ )
            if( board[ i ][ j ] != rhs.board[ i ][ j ] )
                return board[ i ][ j ] < rhs.board[ i ][ j ];
    return false;
}

int TicTacToe::positionValue( ) const {
    return isAWin( COMPUTER ) ? COMPUTER_WIN :
           isAWin( HUMAN )    ? HUMAN_WIN    :
           boardIsFull( )     ? DRAW         : UNCLEAR;
}

int TicTacToe::chooseMove( Side s, int & bestRow, int & bestColumn, int alpha, int beta, int depth ) {
    Side opp;
    int reply, dc, simpleEval, value;
    Position thisPosition = board;
    static const int TABLE_DEPTH = 5;
    if( ( simpleEval = positionValue( ) ) != UNCLEAR ) return simpleEval;
    if( depth == 0 ) transpositions = MapType( );
    else if( depth >= 3 && depth <= TABLE_DEPTH ) {
        MapItr itr = transpositions.find( thisPosition );
        if( itr != transpositions.end( ) ) return (*itr).second;
    }
    if( s == COMPUTER ) opp = HUMAN, value = alpha;
    else opp = COMPUTER, value = beta;
    for( int row = 0; row < 3; row++ )
        for( int column = 0; column < 3; column++ )
            if( squareIsEmpty( row, column ) ) {
                place( row, column, s );
                reply = chooseMove( opp, dc, dc, alpha, beta, depth + 1 );
                place( row, column, EMPTY );
                if( s == COMPUTER && reply > value || s == HUMAN && reply < value ) {
                    if( s == COMPUTER ) alpha = value = reply;
                    else beta = value = reply;
                    bestRow = row, bestColumn = column;
                    if( alpha >= beta ) goto Done;
                }
            }
  Done:
    if( depth <= TABLE_DEPTH ) transpositions[ thisPosition ] = value;
    return value;
}

bool TicTacToe::playMove( Side s, int row, int column ) {
    if( row < 0 || row > 2 || column < 0 || column > 2 || board[ row ][ column ] != EMPTY ) return false;
    board[ row ][ column ] = s;
    return true;
}

void TicTacToe::clearBoard( ) {
    for( int row = 0; row < 3; row++ )
        for( int column = 0; column < 3; column++ )
            board[ row ][ column ] = EMPTY;
}

bool TicTacToe::boardIsFull( ) const {
    for( int row = 0; row < 3; row++ )
        for( int column = 0; column < 3; column++ )
            if( board[ row ][ column ] == EMPTY ) return false;
    return true;
}

bool TicTacToe::isAWin( Side s ) const {
    int row, column;
    for( row = 0; row < 3; row++ ) {
        for( column = 0; column < 3; column++ )
            if( board[ row ][ column ] != s ) break;
        if( column >= 3 ) return true;
    }
    for( column = 0; column < 3; column++ ) {
        for( row = 0; row < 3; row++ )
            if( board[ row ][ column ] != s ) break;
        if( row >= 3 ) return true;
    }
    if( board[ 1 ][ 1 ] == s && board[ 2 ][ 2 ] == s && board[ 0 ][ 0 ] == s ) return true;
    if( board[ 0 ][ 2 ] == s && board[ 1 ][ 1 ] == s && board[ 2 ][ 0 ] == s ) return true;
    return false;
}