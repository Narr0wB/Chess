
#ifndef EVALUATE_H
#define EVALUATE_H

#include <iostream>

#include "position.h"
#include "types.h"

#include "Utils.h"
#include "Transposition.h"
#include "Log.h"

#define INF 500000000

static TranspositionTable table(0x400000);
using namespace std::chrono_literals;

#define MAX_PLY 20

struct SearchHistory {
    Move killer_moves[MAX_PLY][2];
    Move pv_table[MAX_PLY];
    Move tt_move;
    int ply;
    int s_depth;
    uint32_t nodes;
};

// This is the structure that the function search_best_move will return and recursively pass to each of the children nodes
struct SearchInfo {
    uint32_t total_nodes;
    Move best;
    float search_duration_ms;
};


int Evaluate(Position& position, int depth, int search_depth);
int mvv_lva(const Move &m_, const Position &p_);

#define MAX_MOVE_SCORE 10000
int score_move(const Move& m_, const Position& p_, const SearchHistory& s_history_);

// Order moves through shallow (zero depth) evaluations
template <Color Us>
struct move_sorting_criterion {
    const SearchHistory& s_history_;
    const Position& pos_;

    move_sorting_criterion(const Position& p_, const SearchHistory& sh_) : pos_(p_), s_history_(sh_) {}; 

    bool operator() (const Move& a, const Move& b) {
        // if (PV_move != NO_MOVE) {
        //     // If we are ordering the moves in descending order with respect to their scores, then if we find the PV_move
        //     // we need to assign to it the highest priority, which implies that the comparision a > b is false
        //     if (a == PV_move) { return true; } 
            
        //     // If the opposite is true (b is equal to PV_move), then the comparison a > b is false
        //     if (b == PV_move) { return false; }  
        // }

        // int a_relative_score = -1;
        // int b_relative_score = -1;

        // // All capure flags have the first bit set to 1
        // if (a.flags() == MoveFlags::CAPTURE) {
        //     a_relative_score = mvv_lva(a, pos_);
        // }
        // else if (a.flags()) {
            
        // }

        // // All capure flags have the first bit set to 1
        // if (b.flags() == MoveFlags::CAPTURE) {
        //     b_relative_score = mvv_lva(b, pos_);
        // }
        // else {
        //     // TODO: Need to add a way to evaluate quiet moves (quiescence search)
        //     b_relative_score = 0;
        // }

        return score_move(a, pos_, s_history_) > score_move(b, pos_, s_history_);
    }
};

template<Color Us>
void order_move_list(MoveList<Us>& m, const Position& current_pos, SearchHistory& s_history) {
	std::stable_sort(m.begin(), m.end(), move_sorting_criterion<Us>(current_pos, s_history));

    // LOG_INFO("ORDERING... ply {}", s_history.ply) ;
    // for (auto move : m) {
    //     LOG_INFO("Move: {} Score: {}", move, score_move(move, current_pos, s_history));
    // }

    // int scores[m.size()];
    // for (int i = 0; i < m.size(); ++i) {
    //     // if (m.begin()[i] == ) {
    //     //     scores[i] = 1000;
    //     //     continue;
    //     // }
    //     scores[i] = score_move(m.begin()[i], current_pos, s_history);
    // }

    // for (int i = 0; i < m.size(); ++i) {
    //     for (int j = i + 1; j < m.size(); ++j) {
    //         if (scores[i] < scores[j]) {
    //             int tmp = scores[i];
    //             scores[i] = scores[j];
    //             scores[j] = tmp;

    //             auto tmp_move = m.begin()[i];
    //             m.begin()[i] = m.begin()[j];
    //             m.begin()[j] = tmp_move;
    //         }
    //     }
    // }
}


template <Color C>
inline int negamax(Position& board, SearchHistory& hist, int Aalpha, int Bbeta, int depth) {
    hist.nodes++;

    if (depth == 0 || 
        board.checkmate<C>() || 
        board.stalemate<C>()
        // (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_time) > TIME_LIMIT))
    ) 
    {
        int score = Evaluate(board, depth, hist.s_depth) * (C == WHITE ? 1 : -1);
        table.push_position({FLAG_EXACT, board.get_hash(), depth, score, Move(0)});

        return score;
    }
    int ply = hist.s_depth - depth;
    hist.ply = ply;

    Transposition tt_hit = table.probe_hash(board.get_hash(), Aalpha, Bbeta, depth);
    if (tt_hit.score != NO_SCORE) {
        if (tt_hit.score > Aalpha) hist.pv_table[ply] = tt_hit.best;
        return tt_hit.score;
    }
    
    Transposition node_ = Transposition{FLAG_ALPHA, board.get_hash(), depth, NO_SCORE, NO_MOVE};
    
    MoveList<C> mL(board);
    order_move_list(mL, board, hist);

    for (const Move& m : mL) {
        board.play<C>(m);

        int score = -negamax<~C>(board, hist, -Bbeta, -Aalpha, depth - 1);

        board.undo<C>(m);
        //if (ply == 0) LOG_INFO("{} score: {}", m, score);

        if (score > Aalpha) {
            Aalpha = score;

            hist.pv_table[ply] = m;

            node_.flags = FLAG_EXACT;
            node_.score = Aalpha;
            node_.best = m;
        }
        if (Aalpha >= Bbeta) {
            if (m.flags() == MoveFlags::QUIET) {
                hist.killer_moves[ply][1] = hist.killer_moves[ply][0];
                hist.killer_moves[ply][0] = m;
            }

            node_.flags = FLAG_BETA;
            node_.score = Bbeta;
            node_.best = m;

            break;
        }
    }

    table.push_position(node_);

    return Aalpha;
}

template<Color C>
int minimax(Position& board, SearchHistory& hist, int Aalpha, int Bbeta, int depth) {
    hist.nodes++;

    if (depth == 0 || 
        board.checkmate<C>() || 
        board.stalemate<C>()
        // (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_time) > TIME_LIMIT))
    ) 
    {
        int score = Evaluate(board, depth, hist.s_depth);
        table.push_position({FLAG_EXACT, board.get_hash(), 0, score, Move(0)});

        return score;
    }

    int ply = hist.s_depth - depth;
    hist.ply = ply;

    // auto tt = table.probe_hash(board.get_hash(), Aalpha, Bbeta, depth);
    // if (tt.flags != FLAG_EMPTY) {
    //     return tt.score;
    // }

    Transposition node_ = { FLAG_EXACT, board.get_hash(), depth, NO_SCORE, NO_MOVE };

    MoveList<C> mL(board);
    order_move_list<C>(mL, board, hist);

    int best_eval = C == WHITE ? -INF : INF;

    for (const Move& m : mL) {
        //LOG_INFO("Move: {} Score: {} KILLERS: {} {}", m, score_move(m, board, info.search_history), info.search_history.killer_moves[info.search_history.ply][0], info.search_history.killer_moves[info.search_history.ply][1]);
        
        board.play<C>(m);

        int score = NO_SCORE;

        // Recur further down the tree
        if (score == NO_SCORE) {
            score = minimax<~C>(board, hist, Aalpha, Bbeta, depth - 1);
        }

        board.undo<C>(m);

        if (C == WHITE) {
            if (score > best_eval) {
                best_eval = score;
                node_.best = m;
            }
            
            if (score > Aalpha) {
                Aalpha = score;
            }
            if (score >= Bbeta) {
                node_.flags = FLAG_BETA;

                if (m.flags() == MoveFlags::QUIET) {
                    hist.killer_moves[ply][1] = hist.killer_moves[ply][0];
                    hist.killer_moves[ply][0] = m;
                }

                break;
            }
        } 
        else {
            if (score < best_eval) {
                best_eval = score;
                node_.best = m;
            }

            if (score < Bbeta) {
                Bbeta = score;
            }
            if (score <= Aalpha) {
                node_.flags = FLAG_ALPHA;

                if (m.flags() == MoveFlags::QUIET) {
                    hist.killer_moves[ply][1] = hist.killer_moves[ply][0];
                    hist.killer_moves[ply][0] = m;
                }

                break;
            }
        }
    } 

    node_.score = best_eval;
    table.push_position(node_);

    return best_eval;
}

template <Color C>
SearchInfo search_best_move(Position& board, int depth) {
    SearchInfo info = { 0 };

    SearchHistory hist = { 0 };

    auto time_start = std::chrono::system_clock::now();

    int a = -INF;
    int b = INF;

    for (int iteration = 0; iteration <= depth; ++iteration) {
        auto iteration_time_start = std::chrono::system_clock::now();
        hist.s_depth = iteration;

        int score = negamax<C>(board, hist, a, b, iteration);

        // a = score - 50;
        // b = score + 50;

        // if ()

        auto time_end = std::chrono::system_clock::now();
        float iteration_duration = (float) std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start).count() / 1000;

        info.total_nodes += hist.nodes;
        LOG_INFO("pos score: {} depth: {} nodes: {} time: {:.1f}ms pv: ", score, iteration, hist.nodes, iteration_duration);

        hist.nodes = 0;
    }
    // for (const Move& m : mL) {
    //     board.play<C>(m);
    //     int score = minimax<~C>(board, info, a, b, depth);
    //     board.undo<C>(m);

    //     a += 50;
    //     b -= 50;

    //     if (C == BLACK && score < best_score) {
    //         info.best = m;
    //         best_score = score;
    //     }
    //     else if (C == WHITE && score > best_score) {
    //         info.best = m;
    //         best_score = score;
    //     }
    // }

    auto time_end = std::chrono::system_clock::now();
    info.search_duration_ms = (float) std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start).count() / 1000;

    info.best = hist.pv_table[0];

    return info;
}

#endif // EVALUATE_H