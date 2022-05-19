#include <iostream>
#include "Evaluate.h"

int main(int argc, char* argv[]) {

    Chess::Board board(argv[1]);
    // board.movePiece(Tile(0, 0), Tile(3, 3));
    
    // std::cout << board.toFen() << std::endl;
    // std::cout << board.getCurrentPlayer() << std::endl;
    std::pair<Tile, Tile> bM = findBestMove(board, board.getCurrentPlayer());
    std::cout << bM.first.first << bM.first.second
     << bM.second.first << bM.second.second;
    return 0;
}