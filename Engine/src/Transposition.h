
#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#include "types.h"
#include "Utils.h"

#define NO_HASH_ENTRY 0x46464646

#define FLAG_EMPTY 0
#define FLAG_EXACT 1
#define FLAG_ALPHA 2
#define FLAG_BETA 3

typedef uint8_t TranspositionFlags;

struct Transposition {
    uint64_t hash;
    TranspositionFlags flags;

    int score;
    uint8_t depth;
    Move best;
};

class TranspositionTable {
    private:
        Transposition* m_DataArray;
        uint32_t m_Capacity;
        const uint8_t m_BucketSize = 1;

        uint32_t m_Size;
    public:
        TranspositionTable(uint32_t capacity) : m_Capacity(capacity), m_Size(0) {
            m_DataArray = new Transposition[capacity * m_BucketSize];
            memset(m_DataArray, 0, sizeof(Transposition) * m_Capacity * m_BucketSize);
        }

        ~TranspositionTable() {
            delete m_DataArray;
        }

        void push_position(Transposition t);
        int probe_hash(uint64_t hash, int alpha, int beta, int depth);

        inline uint32_t size() { return m_Size; };
};

#endif
