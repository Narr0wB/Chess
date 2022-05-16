#include <iostream>
#include "Chess.h"

int main(int argc, char* argv[]) {

    Chess::Board board(argv[1]);
    //board.movePiece(Tile(0, 0), Tile(3, 3));
    
    std::cout << board.toFen() << std::endl;
    std::cout << vectp(Chess::rookMoves(board, Tile(0, 0)));
    return 0;

}