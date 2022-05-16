#include "Evaluate.h"

std::pair<Tile, Tile> findBestMove(Chess::Board board, uint color) {

    std::pair<Tile, Tile> bestMove;
    float bestMoveScore = 100000000;

    for (Tile t : board.getTiles()) {
        if (board.getPiece(t).color == color) {
            for (Tile move : Chess::generateLegalMoves(board, t)) {
                board.movePiece(t, move);

                float score = moveScore(board, color, false, 0);

                board.movePiece(move, t);

                if (score < bestMoveScore) {
                    bestMove = std::pair<Tile, Tile>(t, move);
                    bestMoveScore = score;
                }
            }
        }
    }

    return bestMove;  
}

float moveScore(Chess::Board board, uint color, bool isMax, uint depth) {
    if (depth == 5) {
        return Evaluate(board, color);
    }

    if (isMax) {
        float best = -1000000000;

        for (Tile t : board.getTiles()) {
            if (board.getPiece(t).color == color)
                for (Tile move : Chess::generateLegalMoves(board, t))
                {   
                    board.movePiece(t, move);
                    float score = moveScore(board, color, !isMax, depth++);
                    best = std::max(best, score);
                    board.movePiece(move, t);
                }
        
        }

        return best;
    }
    else {
        float worst = 1000000000;

        for (Tile t : board.getTiles()) {
            if (board.getPiece(t).color != color && color != -1)
                for (Tile move : Chess::generateLegalMoves(board, t))
                {   
                    board.movePiece(t, move);
                    float score = -moveScore(board, color, !isMax, depth++);
                    worst = std::min(worst, score);
                    board.movePiece(move, t);
                }
        
        }

        return worst;
    }
}


float Evaluate(Chess::Board evaluateBoard, uint color) {
    float score = 0;
    for (Tile t : evaluateBoard.getTiles()) {
        Chess::Piece currentPiece = evaluateBoard.getPiece(t);
        if (currentPiece.color == color && currentPiece.type == 0) {
            score += 500;
        }
        if (currentPiece.color == color && currentPiece.type == 1) {
            score += 400;
        }
        if (currentPiece.color == color && currentPiece.type == 2) {
            score += 100;
        }
        if (currentPiece.color == color && currentPiece.type == 3) {
            score += 80;
        }
        if (currentPiece.color == color && currentPiece.type == 4) {
            score += 100;
        }
        if (currentPiece.color == color && currentPiece.type == 5) {
            score += 10;
        }
    }
}