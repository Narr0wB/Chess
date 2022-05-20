#include "Evaluate.h"



std::pair<Tile, Tile> findBestMove(Chess::Board& board, short color) {

    std::pair<Tile, Tile> bestMove;
    float bestMoveScore = INFINITY;

    for (Tile t : board.getPiecesPositions()) {
        if (board.getPiece(t).color == color) {
            for (Tile move : Chess::generateLegalMoves(board, t)) {
                board.movePiece(t, move);
                
                float score = moveScore(board, 0, color);

                board.movePiece(move, t);

                if (score < bestMoveScore) {
                    bestMove.first = t; 
                    bestMove.second = move;
                    bestMoveScore = score;
                }
            }
        }
    }

    return bestMove;  
}

float moveScore(Chess::Board board, short depth, short color) {
    if (depth == (short) 3) {
        return Evaluate(board);
    }

    depth += 1;
    if (!color) {
        float maxEval = -INFINITY;
        for (Tile t : board.getPiecesPositions()) {
            if (board.getPiece(t).color == color) {
                for (Tile move : Chess::generateLegalMoves(board, t)) {
                    board.movePiece(t, move);
                    float eval = moveScore(board, depth, !color);
                    board.movePiece(move, t);

                    if (eval > maxEval) {
                        maxEval = eval;
                    }
                }
            }
        }

        return maxEval;
        
    }
    else {
        float minEval = INFINITY;
        for (Tile t : board.getPiecesPositions()) {
            if (board.getPiece(t).color == color) {
                for (Tile move : Chess::generateLegalMoves(board, t)) {
                    board.movePiece(t, move);
                    float eval = moveScore(board, depth, !color);
                    board.movePiece(move, t);

                    if (eval < minEval) {
                        minEval = eval;
                    }
                }
            }
        }

        return minEval;
    }

}


float Evaluate(Chess::Board evaluateBoard) {
    static int evaluateCalls = 0;
    float blackScore;
    float whiteScore;
    for (Tile t : evaluateBoard.getPiecesPositions()) {
        Chess::Piece currentPiece = evaluateBoard.getPiece(t);
        if (currentPiece.type == KING) {
            currentPiece.color == WHITE ? whiteScore += 900 : blackScore += 900;
        }
        if (currentPiece.type == QUEEN) {
            currentPiece.color == WHITE ? whiteScore += 90 : blackScore += 90;
        }
        if (currentPiece.type == BISHOP) {
            currentPiece.color == WHITE ? whiteScore += 30 : blackScore += 30;
        }
        if (currentPiece.type == KNIGHT) {
            currentPiece.color == WHITE ? whiteScore += 30 : blackScore += 30;
        }
        if (currentPiece.type == ROOK) {
            currentPiece.color == WHITE ? whiteScore += 50 : blackScore += 50;
        }
        if (currentPiece.type == PAWN) {
            currentPiece.color == WHITE ? whiteScore += 10 : blackScore += 10;
        }
    }

    evaluateCalls += 1;
    //std::cout << evaluateCalls << std::endl;
    return whiteScore - blackScore;
}