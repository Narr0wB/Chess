
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

struct Transposition {
    uint64_t hash;
    TranspositionFlags flags;

    int score;
    uint8_t depth;
    Move best;
};

#define NO_HASH_ENTRY Transposition { 0, 0, NO_SCORE, 0, Move(0) }

class TranspositionTable {
    private:
        Transposition* m_DataArray;

        // The number of transpositions slots available per specific hash (in case of a collision)
        const uint8_t m_BucketSize = 1;

        // Maximum number of transpositions that could be stored in the table
        const uint32_t m_Capacity;

        // Number of transpositions currently stored in the table
        size_t m_Size;

    public:
        TranspositionTable(uint32_t capacity) : m_Capacity(capacity), m_Size(0) {
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
