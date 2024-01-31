
#ifndef EVALUATE_H
#define EVALUATE_H

#include <iostream>

#include "position.h"
#include "types.h"

#include "Transposition.h"
#include "Log.h"

using namespace std::chrono_literals;

// #define MAX_PLY 20

// struct SearchHistory {
//     Move killer_moves[MAX_PLY][2];
//     Move pv_table[MAX_PLY][MAX_PLY];
//     int pv_table_len[MAX_PLY];
//     int h_moves[64][64];
//     int ply;
//     int s_depth;
//     uint32_t nodes;
// };

// This is the structure that the function search_best_move will return and recursively pass to each of the children nodes
// struct SearchInfo {
//     uint32_t total_nodes;
//     Move best;
//     float search_duration_ms;
// };


int Evaluate(Position& position);

// template<Color C>
// int minimax(Position& board, SearchHistory& hist, int Aalpha, int Bbeta, int depth) {
//     hist.nodes++;

//     if (depth == 0 || 
//         board.checkmate<C>() || 
//         board.stalemate<C>()
//         // (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_time) > TIME_LIMIT))
//     ) 
//     {
//         int score = Evaluate(board, depth, hist.s_depth);
//         table.push_position({FLAG_EXACT, board.get_hash(), 0, score, Move(0)});

//         return score;
//     }

//     int ply = hist.s_depth - depth;
//     hist.ply = ply;

//     // auto tt = table.probe_hash(board.get_hash(), Aalpha, Bbeta, depth);
//     // if (tt.flags != FLAG_EMPTY) {
//     //     return tt.score;
//     // }

//     Transposition node_ = { FLAG_EXACT, board.get_hash(), depth, NO_SCORE, NO_MOVE };

//     MoveList<C> mL(board);
//     order_move_list<C>(mL, board, hist);

//     int best_eval = C == WHITE ? -INF : INF;

//     for (const Move& m : mL) {
//         //LOG_INFO("Move: {} Score: {} KILLERS: {} {}", m, score_move(m, board, info.search_history), info.search_history.killer_moves[info.search_history.ply][0], info.search_history.killer_moves[info.search_history.ply][1]);
        
//         board.play<C>(m);

//         int score = NO_SCORE;

//         // Recur further down the tree
//         if (score == NO_SCORE) {
//             score = minimax<~C>(board, hist, Aalpha, Bbeta, depth - 1);
//         }

//         board.undo<C>(m);

//         if (C == WHITE) {
//             if (score > best_eval) {
//                 best_eval = score;
//                 node_.best = m;
//             }
            
//             if (score > Aalpha) {
//                 Aalpha = score;
//             }
//             if (score >= Bbeta) {
//                 node_.flags = FLAG_BETA;

//                 if (m.flags() == MoveFlags::QUIET) {
//                     hist.killer_moves[ply][1] = hist.killer_moves[ply][0];
//                     hist.killer_moves[ply][0] = m;
//                 }

//                 break;
//             }
//         } 
//         else {
//             if (score < best_eval) {
//                 best_eval = score;
//                 node_.best = m;
//             }

//             if (score < Bbeta) {
//                 Bbeta = score;
//             }
//             if (score <= Aalpha) {
//                 node_.flags = FLAG_ALPHA;

//                 if (m.flags() == MoveFlags::QUIET) {
//                     hist.killer_moves[ply][1] = hist.killer_moves[ply][0];
//                     hist.killer_moves[ply][0] = m;
//                 }

//                 break;
//             }
//         }
//     } 

//     node_.score = best_eval;
//     table.push_position(node_);

//     return best_eval;
// }

// template <Color C>
// SearchInfo search_best_move(Position& board, int depth) {
//     SearchInfo info = { 0 };

//     SearchHistory hist = { 0 };
//     for (int i = 0; i < 64; i++) std::memset(hist.h_moves[i], 0, 64 * sizeof(int));

//     auto time_start = std::chrono::system_clock::now();

//     int a = -INF;
//     int b = INF;

//     for (int iteration = 1; iteration <= depth; ++iteration) {
//         auto iteration_time_start = std::chrono::system_clock::now();
//         hist.s_depth = iteration;

//         int score = negamax<C>(board, hist, a, b, iteration);

//         // a = score - 50;
//         // b = score + 50;

//         // if ()

//         auto time_end = std::chrono::system_clock::now();
//         float iteration_duration = (float) std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start).count() / 1000;

//         info.total_nodes += hist.nodes;
//         LOG_INFO("pos score: {} depth: {} nodes: {} time: {:.1f}ms pv: ", score, iteration, hist.nodes, iteration_duration);

//         for (int i = 0; i < depth; ++i) {
//             std::cout << " " << hist.pv_table[0][i] << " ";
//         }
//         std::cout << std::endl;

//         hist.nodes = 0;
//     }

//     auto time_end = std::chrono::system_clock::now();
//     info.search_duration_ms = (float) std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start).count() / 1000;

//     info.best = hist.pv_table[0][0];

//     return info;
// }

#endif // EVALUATE_H