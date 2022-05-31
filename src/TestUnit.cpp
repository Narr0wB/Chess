#include "TestUnit.h"

int moveGenTest(Chess::Board& board, int depth, short color)
{
    if (depth == 0) {
        return 1;
    }

    int numPos = 0;
    depth -= 1;
    for (uint8_t i = 0; i < 8; i++) {
        for (uint8_t g = 0; g < 8; g++) {
            if (board._board[i][g].color != color) {
                continue;
            }

            for (Tile move : Chess::generateLegalMoves(board, Tile(i, g))) {
                Chess::Piece capturedPiece(-1, -1);
                capturedPiece = board._board[move.first][move.second];
                board.movePiece(Tile(i, g), move);
                numPos += moveGenTest(board, depth, !color);
                board.movePiece(move, Tile(i, g));
                board._board[move.first][move.second] = capturedPiece;
            }
        }
    }

    return numPos;
}

int test(std::string& g, int depth) {
    if (depth == 0) {
        return 1;
    }

    int numPos = 0;
    for (char chr : g) {
        numPos += test(g, depth - 1);
    }

    return numPos;
}
