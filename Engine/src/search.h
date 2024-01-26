
#ifndef SEARCH_H 
#define SEARCH_H

#include <iostream>

#include "position.h"
#include "types.h"

#include "Utils.h"
#include "Transposition.h"
#include "Log.h"

#define INF 5000
#define TIME_LIMIT 15 * 1000ms
#define TT_MAX_ENTRIES 0x4000000

// static TranspositionTable _table(TT_MAX_ENTRIES);
#define MAX_PLY 20

struct SearchContext {
    SearchContext (Position& b, int d) : 
    board(b),
    search_depth(d)
    {};

    Position board;
    Move pv_table[MAX_PLY];
    Move killer_moves[2][MAX_PLY];
    int history_moves[64][64];
    int search_depth;
    uint32_t nodes;
};

struct SearchInfo {
    uint32_t total_nodes;
    Move best;
    int score;
    float search_time_ms;
};

namespace Search {

int mvv_lva(const Move &m_, const Position &p_);

#define MAX_MOVE_SCORE 1000000
int score_move(const Move& m_, const SearchContext& ctx);

// Order moves through shallow (zero depth) evaluations
template <Color Us>
struct move_sorting_criterion {
    const SearchContext& _ctx;

    move_sorting_criterion(const Position& p_, const SearchHistory& sh_) : pos_(p_), s_history_(sh_) {}; 

    bool operator() (const Move& a, const Move& b) {
        return score_move(a, pos_, s_history_) > score_move(b, pos_, s_history_);
    }
};

template <Color Us>
void order_move_list(MoveList<Us>& m, const Position& current_pos, SearchHistory& s_history) {
	std::stable_sort(m.begin(), m.end(), move_sorting_criterion<Us>(current_pos, s_history));
}

template <Color C>
int Quiescence(SearchContext& ctx, int Aalpha, int Bbeta) {
    hist_.nodes++;

    int score = Evaluate(board, 0, hist_.s_depth) * (C == WHITE ? 1 : -1);

    if (score >= Bbeta) {
        return Bbeta;
    }

    if (score > Aalpha) {
        Aalpha = score;
    }

    MoveList<C> mL(board);
    order_move_list<C>(mL, board, ctx);

    int best = score;

    for (const Move& m : mL) {
        if (m.flags() != MoveFlags::CAPTURE) continue;

        ctx.board.play<C>(m);

        score = -Quiescence<~C>(ctx, -Bbeta, -Aalpha);

        ctx.board.undo<C>(m);

        if (score > Aalpha) {
            best = score;
            Aalpha = score;
        }

        if (Aalpha >= Bbeta) {
            return Bbeta;
        }
    }

    return best;
}

template <Color C>
inline int negamax(SearchContext& ctx, int Aalpha, int Bbeta, int depth) {
    
    int ply = ctx.search_depth - depth;

    if (depth == 0) {
        int score = Quiescence<C>(ctx, Aalpha, Bbeta);
        table.push_position({FLAG_EXACT, board.get_hash(), depth, score, Move(0)});

        return score;
    }
    
    ctx.nodes++;

    // if (board.checkmate<C>()) {
    //     int score = SHRT_MIN + ply;
    //     table.push_position({FLAG_EXACT, board.get_hash(), depth, score, Move(0)});

    //     return score;
    // }

    // if (board.stalemate<C>()) {
    //     int score = 0;
    //     table.push_position({FLAG_EXACT, board.get_hash(), depth, score, Move(0)});

    //     return score;
    // }

    // Transposition tt_hit = table.probe_hash(board.get_hash(), Aalpha, Bbeta, depth);
    // if (tt_hit.score != NO_SCORE) {
    //     // if (tt_hit.best != NO_MOVE) hist.pv_table[ply] = tt_hit.best;
    //     return tt_hit.score;
    // }
    
    Transposition node_ = Transposition{FLAG_ALPHA, board.get_hash(), depth, NO_SCORE, NO_MOVE};

    MoveList<C> mL(board);
    order_move_list(mL, board, ctx);

    for (const Move& m : mL) {
        ctx.board.play<C>(m);

        int score = -negamax<~C>(board, hist, -Bbeta, -Aalpha, depth - 1);

        ctx.board.undo<C>(m);


        if (score > Aalpha) {
            Aalpha = score;

            ctx.pv_table[ply][ply] = m;
            for (int i = ply + 1; i < ctx.pv_table_len[ply + 1]; ++i) {
                ctx.pv_table[ply][i] = ctx.pv_table[ply + 1][i];
            }
            ctx.pv_table_len[ply] = ctx.pv_table_len[ply + 1];
            
            ctx.h_moves[m.from()][m.to()] += depth;

            node_.flags = FLAG_EXACT;
            node_.score = Aalpha;
            node_.best = m;
        }
        if (Aalpha >= Bbeta) {
            if (m.flags() != MoveFlags::CAPTURE) {
                ctx.killer_moves[ply][1] = hist.killer_moves[ply][0];
                ctx.killer_moves[ply][0] = m;
            }

            node_.flags = FLAG_BETA;
            node_.score = Bbeta;
            node_.best = m;

            table.push_position(node_);

            return Bbeta;
        }
    }

    table.push_position(node_);

    return Aalpha;
}

template <typename C>
SearchInfo Search(Position& _position, int depth, bool debug = false) {
    // Initialize the search context
    SearchContext s_ctx(_position, depth);

    // Initialize the search info result
    SearchInfo s_info;

    int alpha = -INF;
    int beta = INF;

    auto time_start = std::chrono::system_clock::now();

    for (int i = 0; i < depth; ++i) {
        auto iteration_time_start = std::chrono::system_clock::now();

        s_ctx.search_depth = i;
        int score = negamax<C>(s_ctx, alpha, beta, i);

        auto iteration_time_stop = std::chrono::system_clock::now();
        float iteration_duration = (float) std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start).count() / 1000;

        s_info.total_nodes += s_ctx.nodes;
        

        if (debug) {
            LOG_INFO("pos score: {} depth: {} nodes: {} time: {:.1f}ms pv: ", score, i, s_ctx.nodes, iteration_duration);
        }

        s_ctx.nodes = 0;
    }

    auto time_end = std::chrono::system_clock::now();
    s_info.search_duration_ms = (float) std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start).count() / 1000;

    s_info.best = s_ctx.pv_table[0][0];

    return s_info;
}


class Worker {
    private:
    public:
};

} // namespace Search

#endif // SEARCH_H