
#include <iostream>
#include "Chess.h"

std::pair<Tile, Tile> findBestMove(Chess::Board& board, short color);

float moveScore(Chess::Board& board, short color, bool isMax, short depth);

float Evaluate(Chess::Board& evaluateBoard, short color);