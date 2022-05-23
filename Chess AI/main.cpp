#include <iostream>
//#include "Evaluate.h"
#include "TestUnit.h"

int main(int argc, char* argv[]) {

    Chess::Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR/ KQkq w");
    // board.movePiece(Tile(0, 0), Tile(3, 3));
    
    // std::cout << board.toFen() << std::endl;
    //std::cout << vectp(Chess::queenMoves(board, Tile(7, 3))) << std::endl;
    std::pair<Tile, Tile> bM = findBestMove(board, board.getCurrentPlayer());
    std::cout << bM.first.first << bM.first.second
        << bM.second.first << bM.second.second;

    // std::cout << moveGenTest(board, 4, WHITE);
  
    return 0;
}