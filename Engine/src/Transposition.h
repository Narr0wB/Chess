
#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#include <cstring>

#include "types.h"

#define FLAG_EMPTY 0
#define FLAG_EXACT 1
#define FLAG_ALPHA 2
#define FLAG_BETA 3

#define NO_SCORE -0x400000

typedef uint8_t TranspositionFlags;

static int t_hits = 0;

struct Transposition {
    Transposition(TranspositionFlags f, uint64_t h, uint8_t d, int sc, Move b) : 
    flags(f), hash(h), depth(d), score(sc), best(b) {};
    
    Transposition() {};

    TranspositionFlags flags;
    uint64_t hash;
    uint8_t depth;
    int score;
    Move best;
};

#define NO_HASH_ENTRY Transposition { FLAG_EMPTY, 0, 0, NO_SCORE, NO_MOVE }

class TranspositionTable {
    private:
        Transposition* m_DataArray;

        // The number of transpositions slots available per specific hash (in case of a collision)
        const uint8_t m_BucketSize = 1;

        // Maximum number of transpositions that could be stored in the table
        const uint32_t m_Capacity;

        // Number of transpositions currently stored in the table
        size_t m_Size;

        int& hits;

    public:
        TranspositionTable(uint32_t capacity, int& hits) : m_Capacity(capacity), m_Size(0), hits(hits) {
            m_DataArray = new Transposition[capacity * m_BucketSize];
            std::memset(m_DataArray, 0, sizeof(Transposition) * m_Capacity * m_BucketSize);
        }

        ~TranspositionTable() {
            delete m_DataArray;
        }

        void push_position(Transposition t);
        Transposition probe_hash(uint64_t hash, int alpha, int beta, int depth);

        inline uint32_t size() { return m_Size; };
};

#endif
