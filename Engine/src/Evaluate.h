
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
static std::chrono::system_clock::time_point start_time;

static int nodes_searched = 0;
static int search_depth;

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

struct SearchInfo {
    uint32_t nodes;
    uint8_t search_depth;

    Move best;
    Move k_moves[2][248]; 
};

int Evaluate(Position& position, int depth, int search_depth);

int mvv_lva(const Move &m_, const Position &p_);

// Order moves through shallow (zero depth) evaluations
template <Color Us>
struct move_sorting_criterion {
    Move PV_move;
    Position& pos_;

    move_sorting_criterion(Position& p_, Move m_) : pos_(p_), PV_move(m_) {}; 

    bool operator() (const Move& a, const Move& b) {
        if (PV_move != NO_MOVE) {
            // If we are ordering the moves in descending order with respect to their scores, then if we find the PV_move
            // we need to assign to it the highest priority, which implies that the comparision a > b is false
            if (a == PV_move) { return true; } 
            
            // If the opposite is true (b is equal to PV_move), then the comparison a > b is false
            if (b == PV_move) { return false; }  
        }

        int a_relative_score = -1;
        int b_relative_score = -1;

        // All capure flags have the first bit set to 1
        if (a.flags() == MoveFlags::CAPTURE) {
            // pos_ is a member of the move_order_criterion struct
            a_relative_score = mvv_lva(a, pos_);
        }
        else {
            // TODO: Need to add a way to evaluate quiet moves (quiescence search)
            a_relative_score = 0;
        }

        // All capure flags have the first bit set to 1
        if (b.flags() == MoveFlags::CAPTURE) {
            // pos_ is a member of the move_order_criterion struct
            b_relative_score = mvv_lva(b, pos_);
        }
        else {
            // TODO: Need to add a way to evaluate quiet moves (quiescence search)
            b_relative_score = 0;
        }

        return a_relative_score > b_relative_score;
    }
};

template<Color Us>
void order_move_list(MoveList<Us>& m, Position& current_pos) {
    Move PV_move = table.probe_hash(current_pos.get_hash(), 0, 0, 0).best;

	//std::stable_sort(m.begin(), m.end(), move_sorting_criterion<Us>(current_pos, PV_move));

    int scores[m.size()];
    for (int i = 0; i < m.size(); ++i) {
        if (m.begin()[i] == PV_move) {
            scores[i] = 1000;
            continue;
        }
        scores[i] = mvv_lva(m.begin()[i], current_pos);
    }

    for (int i = 0; i < m.size(); ++i) {
        for (int j = i + 1; j < m.size(); ++j) {
            if (scores[i] < scores[j]) {
                int tmp = scores[i];
                scores[i] = scores[j];
                scores[j] = tmp;

                auto tmp_move = m.begin()[i];
                m.begin()[i] = m.begin()[j];
                m.begin()[j] = tmp_move;
            }
        }
    }
}

template<Color C>
int moveScore(Position& board, int Aalpha, int Bbeta, int depth) {
    nodes_searched++;

    if (depth == 0 || 
        board.checkmate<C>() || 
        board.stalemate<C>() ||
        (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_time) > TIME_LIMIT)) {
        
        int score = Evaluate(board, depth, search_depth);
        table.push_position({board.get_hash(), FLAG_EXACT, score, 0, Move(0)});

        return score;
    }

    depth--;

    MoveList<C> mL(board);
    order_move_list<C>(mL, board);

    Transposition node_ = { board.get_hash(), FLAG_EXACT, 0, depth, Move(0)};
    int best_eval = C == WHITE ? -INFINITY : INFINITY;

    for (Move m : mL) {
        board.play<C>(m);

        //int score = table.probe_hash(board.get_hash(), Aalpha, Bbeta, depth).score;
        int score = NO_SCORE;

        if (score == NO_SCORE) {
            score = moveScore<~C>(board, Aalpha, Bbeta, depth);
        }

        board.undo<C>(m);

        if (C == WHITE) {
            if (score > best_eval) {
                best_eval = score;

                node_.score = score;
                node_.best = m;
            }
            
            if (score > Aalpha) {
                Aalpha = score;
            }
            if (score >= Bbeta) {
                node_.flags = FLAG_BETA;
                break;
            }
        } 
        else {
            if (score < best_eval) {
                best_eval = score;

                node_.score = score;
                node_.best = m;
            }

            if (score < Bbeta) {
                Bbeta = score;
            }
            if (score <= Aalpha) {
                node_.flags = FLAG_ALPHA;
                break;
            }
        }
    } 

    table.push_position(node_);
    return best_eval;
}


template <Color C>
Move findBestMove(Position& board, int depth) {
    Move bestMove;
    int bestMoveScore = C == WHITE ? -INFINITY : INFINITY;

    search_depth = depth;
    nodes_searched = 0;

    MoveList<C> mL(board);
    order_move_list<C>(mL, board);

    start_time = std::chrono::system_clock::now();

    for (const Move& m : mL) {
        //LOG_INFO("Move: {} score: {}", m, mvv_lva(m, board));
        board.play<C>(m);
        int score = moveScore<~C>(board, -INFINITY, INFINITY, depth);
        board.undo<C>(m);

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