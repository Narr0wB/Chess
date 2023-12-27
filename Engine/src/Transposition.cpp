
#include "Transposition.h"

void TranspositionTable::push_position(Transposition t)
{
    Transposition& entry = m_DataArray[t.hash % m_Capacity];

    // If the entry is empty then set entry equal to t
    if (entry.flags == FLAG_EMPTY) {
        entry = t;
        m_Size++;
    }


    else {
        // If the entry is not empty first check if its the same position
        if (entry.hash == t.hash) {

            // If the depth of the pushed transposition (t) is bigger (i.e. t's evaluation was due to a bigger search) then replace entry with t
            if (entry.depth < t.depth) {
                entry = t;
            }
        }
        else {
            // TODO: Implement collision safety mechanism
        }
    }
}

int TranspositionTable::probe_hash(uint64_t hash, int alpha, int beta, int depth)
{
    Transposition position = m_DataArray[hash % m_Capacity];

    if (position.hash == hash) {
        if (position.depth >= depth) {
            if (position.flags) {
                return position.score;
            }

            /*if (position.flags == FLAG_ALPHA && position.score <= alpha) {
                return alpha;
            }

            if (position.flags == FLAG_BETA && position.score >= beta) {
                return beta;
            }*/
        }
    }

    return NO_HASH_ENTRY;
}