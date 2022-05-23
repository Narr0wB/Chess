#include "Evaluate.h"



std::pair<Tile, Tile> findBestMove(Chess::Board& board, short color) {

    std::pair<Tile, Tile> bestMove;
    int bestMoveScore = INFINITY;

    for (uint8_t i = 0; i < 8; i++) {
        for (uint8_t g = 0; g < 8; g++) {
            if (board._board[i][g].color == color) {
                for (Tile move : Chess::generateLegalMoves(board, Tile(i, g))) {
                    Chess::Piece capturedPiece(-1, -1);
                    capturedPiece = board._board[move.first][move.second];
                    board.movePiece(Tile(i, g), move);
                    int score = moveScore(board, 0, !color);
                    board.movePiece(move, Tile(i, g));
                    board._board[move.first][move.second] = capturedPiece;

                    if (score < bestMoveScore) {
                        bestMove.first = Tile(i, g); 
                        bestMove.second = move;
                        bestMoveScore = score;
                    }
                }
            }
        }}

    return bestMove;  
}

int moveScore(Chess::Board& board, short depth, short color) {
    if (depth == (short) 4) {
        return Evaluate(board);
    }

    depth += 1;
    if (!color) {
        int maxEval = -INFINITY;
        for (uint8_t i = 0; i < 8; i++) {
            for (uint8_t g = 0; g < 8; g++) {
                if (board._board[i][g].color == color) {
                    for (Tile move : Chess::generateLegalMoves(board, Tile(i, g))) {
                        Chess::Piece capturedPiece(-1, -1);
                        capturedPiece = board._board[move.first][move.second];
                        board.movePiece(Tile(i, g), move);
                        int eval = moveScore(board, depth, !color);
                        board.movePiece(move, Tile(i, g));
                        board._board[move.first][move.second] = capturedPiece;

                        if (eval > maxEval) {
                            maxEval = eval;
                        }
                    }
                }
            }
        }

        return maxEval;
        
    }
    else {
        int minEval = INFINITY;
        for (uint8_t i = 0; i < 8; i++) {
            for (uint8_t g = 0; g < 8; g++) {
                if (board._board[i][g].color == color) {
                    for (Tile move : Chess::generateLegalMoves(board, Tile(i, g))) {
                        Chess::Piece capturedPiece(-1, -1);
                        capturedPiece = board._board[move.first][move.second];
                        board.movePiece(Tile(i, g), move);
                        int eval = moveScore(board, depth, !color);
                        board.movePiece(move, Tile(i, g));
                        board._board[move.first][move.second] = capturedPiece;

                        if (eval < minEval) {
                            minEval = eval;
                        }
                    }
                }
            }
        }

        return minEval;
    }

}


int Evaluate(Chess::Board& evaluateBoard) {
    //static int evaluateCalls = 0;
    int score = 0;
    for (uint8_t i = 0; i < 8; i++) {
        for (uint8_t g = 0; g < 8; g++) {
            short color = evaluateBoard._board[i][g].color;
            if (color == -1)
                continue;
            short multiplier = color ? -1 : 1;
            switch (evaluateBoard._board[i][g].type) {
                case KING: {
                    score += 900 * multiplier;
                }
                case QUEEN: {
                    score += 90 * multiplier;
                }
                case BISHOP: {
                    score += 30 * multiplier;
                }
                case KNIGHT: {
                    score += 30 * multiplier;
                }
                case ROOK: {
                    score += 50 * multiplier;
                }
                case PAWN: {
                    score += 10 * multiplier;
                }
            }
        }
    }

    //evaluateCalls += 1;
    //std::cout << evaluateCalls << std::endl;
    return score;
}