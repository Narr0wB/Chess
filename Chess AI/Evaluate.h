
#include <iostream>
#include "Chess.h"

#define INFINITY 2000000000

std::pair<Tile, Tile> findBestMove(Chess::Board& board, short color);

int moveScore(Chess::Board& board, short depth, short color);

int Evaluate(Chess::Board& evaluateBoard);