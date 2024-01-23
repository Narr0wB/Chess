
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

static TranspositionTable _table(TT_MAX_ENTRIES);

struct SearchContext {};
struct SearchInfo {};



#endif // SEARCH_H