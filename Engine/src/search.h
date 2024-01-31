
#ifndef SEARCH_H 
#define SEARCH_H

#include <iostream>

#include "position.h"
#include "types.h"

#include "Transposition.h"
#include "Evaluate.h"
#include "Log.h"

#define INF 5000000
#define TIME_LIMIT 15 * 1000ms
#define TT_MAX_ENTRIES 0x4000000

static int tt_hits = 0;
static TranspositionTable _table(TT_MAX_ENTRIES, tt_hits);

#define MAX_PLY 20

struct SearchContext {
    SearchContext (Position& b, int d) : 
    board(b),
    search_depth(d)
    {};

    Position board;
    Move pv_table[MAX_PLY][MAX_PLY][MAX_PLY];
    int pv_table_len[MAX_PLY];
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
int score_move(const Move& m_, const SearchContext& ctx, int ply);

// Order moves using context
template <Color Us>
struct move_sorting_criterion {
    const SearchContext& _ctx;
    int _ply;

    move_sorting_criterion(const SearchContext& c, int p) : _ctx(c), _ply(p) {}; 

    bool operator() (const Move& a, const Move& b) {
        return score_move(a, _ctx, _ply) > score_move(b, _ctx, _ply);
    }
};

template <Color Us>
void order_move_list(MoveList<Us>& m, const SearchContext& ctx, int ply) {
	std::stable_sort(m.begin(), m.end(), move_sorting_criterion<Us>(ctx, ply));
}

template <Color C>
int Quiescence(SearchContext& ctx, int Aalpha, int Bbeta, int depth) {
    ctx.nodes++;

    int score = Evaluate(ctx.board) * (C == WHITE ? 1 : -1);

    if (score >= Bbeta) {
        return Bbeta;
    }

    if (score > Aalpha) {
        Aalpha = score;
    }

    if (depth == 0) {
        return Aalpha;
    }

    MoveList<C> mL(ctx.board);

    // If we are in the quiescence search, that means we are at the highest ply, which is equal to the starting search depth
    order_move_list<C>(mL, ctx, ctx.search_depth);

    for (const Move& m : mL) {
        if (m.flags() != MoveFlags::CAPTURE) continue;

        ctx.board.play<C>(m);

        score = -Quiescence<~C>(ctx, -Bbeta, -Aalpha, depth - 1);

        ctx.board.undo<C>(m);

        if (score > Aalpha) {
            Aalpha = score;
        }

        if (Aalpha >= Bbeta) {
            return Bbeta;
        }
    }

    return Aalpha;
}

template <Color C>
inline int negamax(SearchContext& ctx, int Aalpha, int Bbeta, int depth) {

    Transposition tt_hit = _table.probe_hash(ctx.board.get_hash(), Aalpha, Bbeta, depth);
    if (tt_hit.flags != FLAG_EMPTY) {
        return tt_hit.score;
    }
    
    if (depth == 0) {
        int score = Quiescence<C>(ctx, Aalpha, Bbeta, 3);
        _table.push_position({FLAG_EXACT, ctx.board.get_hash(), depth, score, Move(0)});

        return score;
    }
    
    int ply = ctx.search_depth - depth;
    ctx.nodes++;
    
    Transposition node_ = Transposition{FLAG_ALPHA, ctx.board.get_hash(), depth, NO_SCORE, NO_MOVE};

    MoveList<C> mL(ctx.board);

    if (mL.size() == 0) {
        if (ctx.board.in_check<C>()) {
            // If checkmate
            return SHRT_MIN + ply;
        }
        else {
            // If stalemate
            return 0;
        }
    } 

    order_move_list(mL, ctx, ply);

    for (const Move& m : mL) {
        ctx.board.play<C>(m);

        int score = -negamax<~C>(ctx, -Bbeta, -Aalpha, depth - 1);

        ctx.board.undo<C>(m);


        if (score > Aalpha) {
            Aalpha = score;

            // Update the main PV line
            ctx.pv_table[ctx.search_depth][ply][ply] = m;
            for (int i = ply + 1; i < ctx.pv_table_len[ply + 1]; ++i) {
                ctx.pv_table[ctx.search_depth][ply][i] = ctx.pv_table[ctx.search_depth][ply + 1][i];
            }
            ctx.pv_table_len[ply] = ctx.pv_table_len[ply + 1];
            
            // Rank history moves
            if (m.flags() == MoveFlags::QUIET) ctx.history_moves[m.from()][m.to()] += depth;

            node_.flags = FLAG_EXACT;
            node_.best = m;
        }
        if (Aalpha >= Bbeta) {
            if (m.flags() != MoveFlags::CAPTURE) {
                ctx.killer_moves[ply][1] = ctx.killer_moves[ply][0];
                ctx.killer_moves[ply][0] = m;
            }

            node_.flags = FLAG_BETA;
            node_.score = Bbeta;

            _table.push_position(node_);

            return Bbeta;
        }
    }

    node_.score = Aalpha;
    _table.push_position(node_);

    return Aalpha;
}

template <Color C>
SearchInfo Search(Position& _position, int depth, bool debug = false) {
    // Initialize the search context
    SearchContext s_ctx(_position, depth);

    // Initialize the search info result
    SearchInfo s_info;

    int alpha = -INF;
    int beta = INF;

    auto time_start = std::chrono::system_clock::now();

    for (int i = 0; i <= depth; ++i) {
        auto iteration_time_start = std::chrono::system_clock::now();

        // Set the context's search depth to the current depth in the iterative deepening process
        s_ctx.search_depth = i;

        // Score the current position
        int score = negamax<C>(s_ctx, alpha, beta, i);

        // Adjust the window for the next search according to this search's score
        // alpha = score - 50;
        // beta = score + 50;

        // If the current score falls out of the window
        // TODO

        auto iteration_time_stop = std::chrono::system_clock::now();
        float iteration_duration = (float) std::chrono::duration_cast<std::chrono::microseconds>(iteration_time_stop - iteration_time_start).count() / 1000;

        s_info.total_nodes += s_ctx.nodes;
        
        if (debug) {
            LOG_INFO("pos score: {} depth: {} nodes: {} time: {:.1f}ms t_hits: {}", score, i, s_ctx.nodes, iteration_duration, tt_hits);
        }

        tt_hits = 0;

        s_ctx.nodes = 0;
    }

    auto time_end = std::chrono::system_clock::now();
    s_info.search_time_ms = (float) std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start).count() / 1000;

    s_info.best = s_ctx.pv_table[depth][0][0];

    return s_info;
}


class Worker {
    private:
    public:
};

} // namespace Search

#endif // SEARCH_H