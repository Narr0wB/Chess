
#include <iostream>
#include "position.h"
#include "types.h"
#include "Utils.h"

#define INFINITY 2000000000

const int black_pawn_table[64] = {  0,  0,  0,  0,  0,  0,  0,  0,
                                   50, 50, 50, 50, 50, 50, 50, 50,
                                   10, 10, 20, 30, 30, 20, 10, 10,
                                    5,  5, 10, 25, 25, 10,  5,  5,
                                    0,  0,  0, 20, 20,  0,  0,  0,
                                    5, -5,-10,  0,  0,-10, -5,  5,
                                    5, 10, 10,-20,-20, 10, 10,  5,
                                    0,  0,  0,  0,  0,  0,  0,  0 };

const int white_pawn_table[64] = { 0,  0,  0,  0,  0,  0,  0,  0,
                                   5, 10, 10,-20,-20, 10, 10,  5,
                                   5, -5,-10,  0,  0,-10, -5,  5,
                                   0,  0,  0, 20, 20,  0,  0,  0,
                                   5,  5, 10, 25, 25, 10,  5,  5,
                                   10, 10, 20, 30, 30, 20, 10, 10,
                                   50, 50, 50, 50, 50, 50, 50, 50,
                                    0,  0,  0,  0,  0,  0,  0,  0 };

const int knight_table[64] = { -50,-40,-30,-30,-30,-30,-40,-50,
                               -40,-20,  0,  0,  0,  0,-20,-40,
                               -30,  0, 10, 15, 15, 10,  0,-30,
                               -30,  5, 15, 20, 20, 15,  5,-30,
                               -30,  0, 15, 20, 20, 15,  0,-30,
                               -30,  5, 10, 15, 15, 10,  5,-30,
                               -40,-20,  0,  5,  5,  0,-20,-40,
                               -50,-40,-30,-30,-30,-30,-40,-50 };

const int black_bishop_table[64] = { -20,-10,-10,-10,-10,-10,-10,-20,
                                     -10,  0,  0,  0,  0,  0,  0,-10,
                                     -10,  0,  5, 10, 10,  5,  0,-10,
                                     -10,  5,  5, 10, 10,  5,  5,-10,
                                     -10,  0, 10, 10, 10, 10,  0,-10,
                                     -10, 10, 10, 10, 10, 10, 10,-10,
                                     -10,  5,  0,  0,  0,  0,  5,-10,
                                     -20,-10,-10,-10,-10,-10,-10,-20 };

const int white_bishop_table[64] = { -20,-10,-10,-10,-10,-10,-10,-20,
                                     -10,  5,  0,  0,  0,  0,  5,-10,
                                     -10, 10, 10, 10, 10, 10, 10,-10,
                                     -10,  0, 10, 10, 10, 10,  0,-10,
                                     -10,  5,  5, 10, 10,  5,  5,-10,
                                     -10,  0,  5, 10, 10,  5,  0,-10,
                                     -10,  0,  0,  0,  0,  0,  0,-10,
                                     -20,-10,-10,-10,-10,-10,-10,-20 };

const int black_rook_table[64] = { 0,  0,  0,  0,  0,  0,  0,  0,
                                   5, 10, 10, 10, 10, 10, 10,  5,
                                  -5,  0,  0,  0,  0,  0,  0, -5,
                                  -5,  0,  0,  0,  0,  0,  0, -5,
                                  -5,  0,  0,  0,  0,  0,  0, -5,
                                  -5,  0,  0,  0,  0,  0,  0, -5,
                                  -5,  0,  0,  0,  0,  0,  0, -5,
                                   0,  0,  0,  5,  5,  0,  0,  0 };

const int white_rook_table[64] = { 0,  0,  0,  5,  5,  0,  0,  0,
                                 - 5,  0,  0,  0,  0,  0,  0, -5,
                                  -5,  0,  0,  0,  0,  0,  0, -5,
                                  -5,  0,  0,  0,  0,  0,  0, -5,
                                  -5,  0,  0,  0,  0,  0,  0, -5,
                                  -5,  0,  0,  0,  0,  0,  0, -5, 
                                   5, 10, 10, 10, 10, 10, 10,  5, 
                                   0,  0,  0,  0,  0,  0,  0,  0, };

const int black_queen_table[64] = { -20,-10,-10, -5, -5,-10,-10,-20,
                                    -10,  0,  0,  0,  0,  0,  0,-10,
                                    -10,  0,  5,  5,  5,  5,  0,-10,
                                     -5,  0,  5,  5,  5,  5,  0, -5,
                                      0,  0,  5,  5,  5,  5,  0, -5,
                                    -10,  5,  5,  5,  5,  5,  0,-10,
                                    -10,  0,  5,  0,  0,  0,  0,-10,
                                    -20,-10,-10, -5, -5,-10,-10,-20 };

const int white_queen_table[64] = { -20,-10,-10, -5, -5,-10,-10,-20,
                                    -10,  0,  5,  0,  0,  0,  0,-10, 
                                    -10,  5,  5,  5,  5,  5,  0,-10,
                                      0,  0,  5,  5,  5,  5,  0, -5,
                                     -5,  0,  5,  5,  5,  5,  0, -5,
                                    -10,  0,  5,  5,  5,  5,  0,-10,
                                    -10,  0,  0,  0,  0,  0,  0,-10,
                                    -20,-10,-10, -5, -5,-10,-10,-20, };

const int black_king_mg_table[64] = { -30,-40,-40,-50,-50,-40,-40,-30,
                                      -30,-40,-40,-50,-50,-40,-40,-30,
                                      -30,-40,-40,-50,-50,-40,-40,-30,
                                      -30,-40,-40,-50,-50,-40,-40,-30,
                                      -20,-30,-30,-40,-40,-30,-30,-20,
                                      -10,-20,-20,-20,-20,-20,-20,-10,
                                       20, 20,  0,  0,  0,  0, 20, 20,
                                       20, 30, 10,  0,  0, 10, 30, 20 };

const int white_king_mg_table[64] = { 20, 30, 10,  0,  0, 10, 30, 20,
                                      20, 20,  0,  0,  0,  0, 20, 20, 
                                     -10,-20,-20,-20,-20,-20,-20,-10, 
                                     -20,-30,-30,-40,-40,-30,-30,-20, 
                                     -30,-40,-40,-50,-50,-40,-40,-30, 
                                     -30,-40,-40,-50,-50,-40,-40,-30, 
                                     -30,-40,-40,-50,-50,-40,-40,-30, 
                                     -30,-40,-40,-50,-50,-40,-40,-30, };

const int black_king_eg_table[64] = { -50,-40,-30,-20,-20,-30,-40,-50,
                                      -30,-20,-10,  0,  0,-10,-20,-30,
                                      -30,-10, 20, 30, 30, 20,-10,-30,
                                      -30,-10, 30, 40, 40, 30,-10,-30,
                                      -30,-10, 30, 40, 40, 30,-10,-30,
                                      -30,-10, 20, 30, 30, 20,-10,-30,
                                      -30,-30,  0,  0,  0,  0,-30,-30,
                                      -50,-30,-30,-30,-30,-30,-30,-50 };

const int white_king_eg_table[64] = { -50,-30,-30,-30,-30,-30,-30,-50,
                                      -30,-30,  0,  0,  0,  0,-30,-30,
                                      -30,-10, 20, 30, 30, 20,-10,-30,
                                      -30,-10, 30, 40, 40, 30,-10,-30,
                                      -30,-10, 30, 40, 40, 30,-10,-30,
                                      -30,-10, 20, 30, 30, 20,-10,-30,
                                      -30,-20,-10,  0,  0,-10,-20,-30,
                                      -50,-40,-30,-20,-20,-30,-40,-50, };

int newEvaluate(Position& eB);

int Evaluate(Position& evaluateBoard);

int piece_to_idx(Piece& p);
int piece_color(Piece& p);
int piece_value(bool mg, Piece& square);
int psqt_value(bool mg, Piece& square, int sq);
int imbalance(Piece& square, Piece* pieces);
int imbalance_t(Piece& square, Piece* pieces);
int middle_game_eval(Position& b);

static int mSCalls = 0;
template<Color color>
int moveScore(Position& board, int Aalpha, int Bbeta, int depth) {
    ++mSCalls;
    MoveList<color> mL(board);
    if (depth == 0 || mL.size() == 0) {
        return Evaluate(board);
    }


    depth -= 1;
    if (color == WHITE) {
        int maxEval = -INFINITY;
        for (Move m : mL) {
            board.play<color>(m);
            int eval = moveScore<~color>(board, Aalpha, Bbeta, depth);
            board.undo<color>(m);

            if (eval > maxEval)
                maxEval = eval;
            if (eval > Aalpha)
                Aalpha = eval;

            if (eval >= Bbeta)
                break;
        }

        return maxEval;
    }
    else {
        int minEval = INFINITY;
        for (Move m : mL) {
            board.play<color>(m);
            int eval = moveScore<~color>(board, Aalpha, Bbeta, depth);
            board.undo<color>(m);

            if (eval < minEval)
                minEval = eval;
            if (eval < Bbeta)
                Bbeta = eval;

            if (eval <= Aalpha)
                break;
        }

        return minEval;
    }

}


template <Color color>
Move findBestMove(Position& board, int depth) {

    Move bestMove;
    int bestMoveScore = color == WHITE ? -INFINITY : INFINITY;


    MoveList<color> mL(board);

    for (Move m : mL) {
        board.play<color>(m);
        int score = moveScore<~color>(board, -INFINITY, INFINITY, depth);
        board.undo<color>(m);

        if (color == BLACK && score < bestMoveScore) {
            bestMove = m;
            bestMoveScore = score;
        }
        else if (color == WHITE && score > bestMoveScore) {
            bestMove = m;
            bestMoveScore = score;
        }
    }


    return bestMove;
}