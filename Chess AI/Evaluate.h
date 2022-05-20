
#include <iostream>
#include "Chess.h"

#define INFINITY 3.40282e+038

std::pair<Tile, Tile> findBestMove(Chess::Board& board, short color);

float moveScore(Chess::Board board, short depth, short color);

float Evaluate(Chess::Board evaluateBoard);