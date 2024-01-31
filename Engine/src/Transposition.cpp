
#include "Transposition.h"
#include "Log.h"

void TranspositionTable::push_position(Transposition t)
{
    Transposition& entry = m_DataArray[t.hash % m_Capacity];

    // // If the entry is empty then set entry equal to t
    entry = t;
//     if (entry.flags == FLAG_EMPTY) {
//         m_Size++;
//     }
//     else {
//         entry = t;
//         // If the entry is not empty first check if its the same position
//         // if (entry.hash == t.hash) {

//         //     // If the depth of the transposition (t) is higher
//         //     // then replace entry with t
//         //     if (t.depth > entry.depth) {
//         //         entry = t;
//         //     }
//         // }
//         // else {
//         //     // if (t.depth > entry.depth) {
//         //     //     entry = t;
//         //     // }
//         // }
//     }
}

Transposition TranspositionTable::probe_hash(uint64_t hash, int alpha, int beta, int depth) {
    Transposition position = m_DataArray[hash % m_Capacity];

    if (position.hash == hash) {
        if (position.depth >= depth) {
            if (position.flags == FLAG_EXACT) {
                return position;
            }

            if (position.flags == FLAG_ALPHA && position.score <= alpha) {
                position.score = alpha;
                return position;
            }

            if (position.flags == FLAG_BETA && position.score >= beta) {
                position.score = beta;
                return position;
            }

            hits++;
        }
    }

    return NO_HASH_ENTRY;
}