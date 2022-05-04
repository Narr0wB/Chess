
#include <stdio.h>
#include "Chess.h"

std::pair<Tile, Tile> findBestMove(Chess::Board board, uint color);

float moveScore(Chess::Board board, uint color, bool isMax, uint depth);

float Evaluate(Chess::Board evaluateBoard, uint color);