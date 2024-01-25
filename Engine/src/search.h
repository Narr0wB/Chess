
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
    Position board;
    Move pv_table[MAX_PLY];
    Move killer_moves[2][MAX_PLY];
    int history_moves[64][64];
    int search_depth;
    int current_ply;
    uint32_t nodes;
};

struct SearchInfo {
    uint32_t total_nodes;
    Move best;
    int score;
    float search_time_ms;
};

namespace Search {



class Worker {
    private:
    public:
};

} // namespace Search

#endif // SEARCH_H