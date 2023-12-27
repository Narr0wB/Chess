
#ifndef EVALUATE_H
#define EVALUATE_H

#include <iostream>

#include "position.h"
#include "types.h"

#include "Utils.h"
#include "Transposition.h"
#include "Log.h"

#define INFINITY 2000000000
#define TIME_LIMIT 15 * 1000ms

using namespace std::chrono_literals;

static TranspositionTable table(0x4000000);
static int nodes_searched = 0;
static int search_depth = 4;
static std::chrono::system_clock::time_point start_time;

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

int Evaluate(Position& position, int depth);

int piece_to_idx(Piece& p);
int piece_color(Piece& p);
int piece_value(bool mg, Piece& square);
int psqt_value(bool mg, Piece& square, int sq);
int imbalance(Piece& square, Piece* pieces);
int imbalance_t(Piece& square, Piece* pieces);
int middle_game_eval(Position& b);

template<Color C>
int moveScore(Position& board, int Aalpha, int Bbeta, int depth) {
    nodes_searched++;
    TranspositionFlags flags = FLAG_EXACT;

    if (depth == 0 || 
        board.checkmate<C>() || 
        board.stalemate<C>() ||
        (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_time) > TIME_LIMIT)) {
        int score = Evaluate(board, depth);
        
        // if (board.checkmate<C>()) { std::cout << board << std::endl; std::cout << depth << std::endl; }

        table.push_position({board.get_hash(), flags, score, 0, Move(0)});
        return score;
    }

    MoveList<C> mL(board);

    depth--;

    if (C == WHITE) {
        int maxEval = -INFINITY;

        for (Move m : mL) {
            board.play<C>(m);

            int score = table.probe_hash(board.get_hash(), Aalpha, Bbeta, depth);

            if (score == NO_HASH_ENTRY) {
                score = moveScore<~C>(board, Aalpha, Bbeta, depth);
            }

            board.undo<C>(m);

            if (score > maxEval)
                maxEval = score;
            if (score > Aalpha)
                Aalpha = score;

            if (score >= Bbeta) {
                flags = FLAG_BETA;
                break;
            }
        }

        table.push_position(Transposition{ board.get_hash(), flags, maxEval, (uint8_t) depth, Move(0)});

        return maxEval;
    }
    else {
        int minEval = INFINITY;
        for (Move m : mL) {
            board.play<C>(m);

            int score = table.probe_hash(board.get_hash(), Aalpha, Bbeta, depth);

            if (score == NO_HASH_ENTRY) {
                score = moveScore<~C>(board, Aalpha, Bbeta, depth);
            }

            board.undo<C>(m);

            if (score < minEval)
                minEval = score;
            if (score < Bbeta)
                Bbeta = score;

            if (score <= Aalpha) {
                flags = FLAG_ALPHA;
                break;
            }
        }

        table.push_position(Transposition{ board.get_hash(), flags, minEval, (uint8_t) depth, Move(0) });

        return minEval;
    }

}


template <Color C>
Move findBestMove(Position& board, int depth) {

    Move bestMove;
    int bestMoveScore = C == WHITE ? -INFINITY : INFINITY;

    search_depth = depth;

    MoveList<C> mL(board);
    start_time = std::chrono::system_clock::now();

    for (Move m : mL) {
        board.play<C>(m);
        int score = moveScore<~C>(board, -INFINITY, INFINITY, depth);
        board.undo<C>(m);

        // if (C == WHITE) std::cout << m << " " << score << std::endl;

        if (C == BLACK && score < bestMoveScore) {
            bestMove = m;
            bestMoveScore = score;
        }
        else if (C == WHITE && score > bestMoveScore) {
            bestMove = m;
            bestMoveScore = score;
        }
    }

    return bestMove;
}

#endif // EVALUATE_H